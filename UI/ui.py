import sys

from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtCore import QThread
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QDoubleValidator
from PyQt5.QtWidgets import QLineEdit
from PyQt5.QtWidgets import (QWidget, QToolTip, 
    QPushButton, QApplication, QLabel, QGridLayout,QVBoxLayout,QHBoxLayout,QRadioButton)
from PyQt5.QtGui import QFont   
import pyqtgraph as pg
import random
import time
import numpy as np
import random
import collections
import socket
import threading
import math
from pyqtgraph.Qt import QtGui, QtCore
import serial

import design

import sys
from struct import *


def dq0_transform(v_a, v_b, v_c):
    # TODO(akremor): Check this transform
    d = (np.sqrt(2/3)*v_a-(1/(np.sqrt(6)))*v_b-(1/(np.sqrt(6)))*v_c)
    q = ((1/(np.sqrt(2)))*v_b-(1/(np.sqrt(2)))*v_c)
    return d, q

class LiveFFTWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(LiveFFTWidget, self).__init__(parent)

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(100)
        QtGui.QWidget.__init__(self)

    def initUI(self):
        self.buffer_size = 1000
        self.timewindow = 10

        self.fft_data = np.zeros(self.buffer_size, dtype=np.float)
        self.freq_vect = np.zeros(self.buffer_size, dtype=np.float)

        self.lay = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.setLabel('left', 'Amplitude', 'A')
        self.plotter.setLabel('bottom', 'Frequency', 'Hz')
        self.curve = self.plotter.plot(self.freq_vect, self.fft_data, pen=(255,0,0))
        self.lay.addWidget(self.plotter)

    def updateplot(self):
        self.curve.setData(self.freq_vect, self.fft_data)

    @pyqtSlot(object, object)
    def take_sample(self, freq_vect, fft_data):
        self.freq_vect = freq_vect
        self.fft_data = fft_data


class LivePlotWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(LivePlotWidget, self).__init__(parent)

        self.givedata_buffer = None

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(100)

    def initUI(self, n_plots, legends, x_axis, y_axis):
        self.buffer_size = 100
        self.curves = [None] * n_plots
        self.legends = legends
        self.x_axis = x_axis
        self.y_axis = y_axis
        self.n_plots = n_plots
        self.timewindow = 10
        self.prev_time = time.time() * 1000
        #self.x = collections.deque([0.0]*self.buffer_size, self.buffer_size)
        self.x = np.linspace(-10,0,self.buffer_size)
        self.y = [collections.deque([0.0]*self.buffer_size, self.buffer_size)] * n_plots
        self.databuffer = [collections.deque([0.0]*self.buffer_size, self.buffer_size)] * n_plots
        self.givedata_buffer = None

        self.layout = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.setLabel('left', *self.y_axis)
        self.plotter.setLabel('bottom', *self.x_axis)
        colours = [(255, 255, 255), (255, 0, 0), (0, 255, 0)]
        for i in range(self.n_plots):
            self.curves[i] = self.plotter.plot(self.x, self.y[i], pen=colours[i])
        self.layout.addWidget(self.plotter)

    def updateplot(self):

        if self.givedata_buffer is None:
            return

        for i in range(self.n_plots):
            self.databuffer[i].append(self.givedata_buffer[i])
            self.curves[i].setData(self.x, self.databuffer[i])

    # TODO How to make this responsive to number of things?
    @QtCore.pyqtSlot(float, float, float)
    def take_sample(self, a, b, c):
        self.givedata_buffer = [a, b, c]


class SerialReader(QObject):

    ref_voltage_dq_signal = pyqtSignal(float, float, float, name='ref_voltage_dq')
    ref_current_dq_signal = pyqtSignal(float, float, float, name='ref_current_dq')

    error_voltage_dq_signal = pyqtSignal(float, float, float, name='error_voltage_dq')
    error_current_dq_signal = pyqtSignal(float, float, float, name='error_current_dq')

    sensed_current_abc_signal = pyqtSignal(float, float, float, name='sensed_current_abc')
    sensed_current_dq_signal = pyqtSignal(float, float, float, name='sensed_current_dq')
    sensed_voltage_abc_signal = pyqtSignal(float, float, float, name='sensed_voltage_abc')
    sensed_voltage_dq_signal = pyqtSignal(float, float, float, name='sensed_voltage_dq')

    sensed_voltage_fft_signal = pyqtSignal(object, object, name='sensed_voltage_fft')
    sensed_current_fft_signal = pyqtSignal(object, object, name='sensed_current_fft')

    def __init__(self, serial_port, buffer_size = 1000):
        super().__init__()
        self.buffer_size = buffer_size
        self.sensed_voltage_sample_buffer = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.sensed_current_sample_buffer = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.serial_port = serial_port

    @pyqtSlot()
    def run(self):
        while False:
            self.serial_port.read_until(b'\x41\x61')

            # We have now achieved sync
            # Make next char include size
            length = self.serial_port.read(1)

            length = int.from_bytes(length, byteorder='little')

            data = self.serial_port.read(length)
            data = unpack('ffffffffff', data)

            print("Read {} bytes".format(len(data)))
            print(data)

            data = [a] * 10
            # Unpack the data
            Id_ref = data[0]
            Iq_ref = data[1]
            I_Aa = data[2]
            I_Bb = data[3]
            I_Cc = data[4]
            V_an = data[5]
            V_bn = data[6]
            V_cn = data[7]
            Id_error = data[8]
            Iq_error = data[9]

        while True:
            time.sleep(0.01)

            I_Aa = 1 * math.sin(time.time() * 2 * 1 * math.pi)
            I_Bb = 1 * math.sin(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
            I_Cc = 1 * math.sin(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

            V_an = 1 * math.cos(time.time() * 2 * 1 * math.pi)
            V_bn = 1 * math.cos(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
            V_cn = 1 * math.cos(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

            self.sensed_voltage_sample_buffer.append(V_an)
            self.sensed_current_sample_buffer.append(I_Aa)

            I_d, I_q = dq0_transform(I_Aa, I_Bb, I_Cc)
            V_d, V_q = dq0_transform(V_an, V_bn, V_cn)

            voltage_fft_data = np.abs(np.fft.rfft(self.sensed_voltage_sample_buffer))
            current_fft_data = np.abs(np.fft.rfft(self.sensed_current_sample_buffer))
            freq_vect = np.fft.rfftfreq(1000, 1000)

            self.sensed_current_abc_signal.emit(I_Aa, I_Bb, I_Cc)
            self.sensed_voltage_abc_signal.emit(V_an, V_bn, V_cn)

            self.sensed_current_dq_signal.emit(I_d, I_q, 0)
            self.sensed_voltage_dq_signal.emit(V_d, V_q, 0)

            self.sensed_voltage_fft_signal.emit(freq_vect, voltage_fft_data)
            self.sensed_current_fft_signal.emit(freq_vect, current_fft_data)



            app.processEvents()


class InverterApp(QtGui.QMainWindow, design.Ui_MainWindow):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        serial_port = None  # serial.Serial('COM9', 921600)

        self.serial_read_worker = SerialReader(serial_port)
        self.serial_read_thread = QThread()
        self.serial_read_worker.moveToThread(self.serial_read_thread)
        self.serial_read_thread.started.connect(self.serial_read_worker.run)

        self.serial_read_worker.ref_voltage_dq_signal.connect(self.ref_voltage_dq.take_sample)
        self.serial_read_worker.ref_current_dq_signal.connect(self.ref_current_dq.take_sample)

        self.serial_read_worker.error_voltage_dq_signal.connect(self.error_voltage_dq.take_sample)
        self.serial_read_worker.error_current_dq_signal.connect(self.error_current_dq.take_sample)

        self.serial_read_worker.sensed_current_abc_signal.connect(self.sensed_current_abc.take_sample)
        self.serial_read_worker.sensed_current_dq_signal.connect(self.sensed_current_dq.take_sample)
        self.serial_read_worker.sensed_voltage_abc_signal.connect(self.sensed_voltage_abc.take_sample)
        self.serial_read_worker.sensed_voltage_dq_signal.connect(self.sensed_voltage_dq.take_sample)

        self.serial_read_worker.sensed_voltage_fft_signal.connect(self.sensed_voltage_fft.take_sample)
        self.serial_read_worker.sensed_current_fft_signal.connect(self.sensed_current_fft.take_sample)

        self.ref_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))
        self.ref_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))

        self.error_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))
        self.error_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))

        self.sensed_current_abc.initUI(3, ['a', 'b', 'c'], ('Time', 's'), ('Current', 'A'))
        self.sensed_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))
        self.sensed_voltage_abc.initUI(3, ['a', 'b', 'c'], ('Time', 's'), ('Voltage', 'V'))
        self.sensed_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))

        self.sensed_voltage_fft.initUI()
        self.sensed_current_fft.initUI()

        self.serial_read_thread.start()

    def run(self):
        self.show()


if __name__ == '__main__':
    
    app = QtGui.QApplication([])
    plt = InverterApp()

    plt.run()

    sys.exit(app.exec_())


