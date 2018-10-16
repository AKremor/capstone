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

import sys
from struct import *

class LiveFFTWidget(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.buffer_size = 1000
        self.timewindow = 10

        self.fft_data = np.zeros(self.buffer_size, dtype=np.float)
        self.freq_vect = np.zeros(self.buffer_size, dtype=np.float)

        self.initUI()

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(100)

    def initUI(self):
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

class LiveSVMWidget(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        # customize the UI
        self.initUI()

        self.ref_vector_tip_x = 0
        self.ref_vector_tip_y = 0

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(1)

    def initUI(self):
        self.lay = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.curve = self.plotter.plot([], pen=None, 
            symbolBrush=(255,0,0), symbolSize=5, symbolPen=None)

        self.ref_vector_curve = self.plotter.plot([0, 0], [0, 0], pen=(255,0,0))
        self.plotter.setXRange(-10, 10, padding=2)
        self.plotter.setYRange(-10, 10, padding=2)
        self.lay.addWidget(self.plotter)

    def set_ref_vector(self, ref_x, ref_y):
        self.ref_vector_tip_x = ref_x
        self.ref_vector_tip_y = ref_y

    def updateplot(self):
        self.ref_vector_curve.setData([0, self.ref_vector_tip_x], [0, self.ref_vector_tip_y])


class LivePlotWidget(QtGui.QWidget):
    def __init__(self, n_plots, legends, x_axis, y_axis):

        QtGui.QWidget.__init__(self)
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
        
        # customize the UI
        self.initUI()

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(100)

    def initUI(self):
        self.lay = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.setLabel('left', *self.y_axis)
        self.plotter.setLabel('bottom', *self.x_axis)
        colours = [(255, 255, 255), (255, 0, 0), (0, 255, 0)]
        for i in range(self.n_plots):
            self.curves[i] = self.plotter.plot(self.x, self.y[i], pen=colours[i])
        self.lay.addWidget(self.plotter)

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


class Inverter(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.sensed_current_plot = LivePlotWidget(3, ['a', 'b', 'c'], ('Time', 's'), ('Current', 'A'))
        self.sensed_voltage_plot = LivePlotWidget(3, ['a', 'b', 'c'], ('Time', 's'), ('Voltage', 'V'))

        self.initUI()

    def initUI(self):
        grid = QGridLayout()
        grid.setSpacing(10)

        self.sensed_V_FFT_plot = LiveFFTWidget()
        self.sensed_I_FFT_plot = LiveFFTWidget()

        sensed_voltage_label = QLabel()
        sensed_voltage_label.setText("Sensed Voltage")
        sensed_voltage_label.setAlignment(QtCore.Qt.AlignCenter)
        sensed_current_label = QLabel()
        sensed_current_label.setText("Sensed Current")
        sensed_current_label.setAlignment(QtCore.Qt.AlignCenter)

        sensed_voltage_FFT_label = QLabel()
        sensed_voltage_FFT_label.setText("Sensed Voltage FFT")
        sensed_voltage_FFT_label.setAlignment(QtCore.Qt.AlignCenter)
        sensed_current_FFT_label = QLabel()
        sensed_current_FFT_label.setText("Sensed Current FFT")
        sensed_current_FFT_label.setAlignment(QtCore.Qt.AlignCenter)

        grid.addWidget(sensed_voltage_label, 1, 1, 1, 2)
        grid.addWidget(self.sensed_voltage_plot, 2, 1, 1, 2)
        grid.addWidget(sensed_current_label, 1, 3, 1, 2)
        grid.addWidget(self.sensed_current_plot, 2, 3, 1, 2)

        grid.addWidget(self.sensed_V_FFT_plot, 4, 1, 1, 2)
        grid.addWidget(sensed_voltage_FFT_label, 3, 1, 1, 2)
        grid.addWidget(self.sensed_I_FFT_plot, 4, 3, 1, 2)
        grid.addWidget(sensed_current_FFT_label, 3, 3, 1, 2)

        self.setLayout(grid) 
        
        self.resize(900, 600)
        self.setWindowTitle('Inverter')    
    
    def run(self):
        self.show()

class SerialReader(QObject):

    sensed_I_abc = pyqtSignal(float, float, float, name='sensed_I_abc')
    sensed_V_abc = pyqtSignal(float, float, float, name='sensed_V_abc')

    sensed_V_FFT = pyqtSignal(object, object, name="sensed_V_FFT")
    sensed_I_FFT = pyqtSignal(object, object, name="sensed_I_FFT")

    new_fft_a_sample = pyqtSignal(float)

    def __init__(self, serial_port, buffer_size = 1000):
        super().__init__()
        self.buffer_size = buffer_size
        self.sensed_V_sample_buffer = [collections.deque([0.0] * self.buffer_size, self.buffer_size)]*3
        self.sensed_I_sample_buffer = [collections.deque([0.0] * self.buffer_size, self.buffer_size)]*3
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

        while True:
            a = 1 * math.sin(time.time() * 2 * 0.001 * math.pi)
            b = 1 * math.sin(time.time() * 2 * 0.001 * math.pi - 2 * math.pi / 3)
            c = 1 * math.sin(time.time() * 2 * 0.001 * math.pi + 2 * math.pi / 3)

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

            time.sleep(0.01)
            I_Aa = a
            I_Bb = b
            I_Cc = c

            V_an = a
            V_bn = b
            V_cn = c

            self.sensed_I_sample_buffer[0].append(I_Aa)
            self.sensed_I_sample_buffer[1].append(I_Bb)
            self.sensed_I_sample_buffer[2].append(I_Cc)
            self.sensed_V_sample_buffer[0].append(V_an)
            self.sensed_V_sample_buffer[1].append(V_bn)
            self.sensed_V_sample_buffer[2].append(V_cn)

            fft_data = np.abs(np.fft.rfft(self.sensed_I_sample_buffer[0]))
            freq_vect = np.fft.rfftfreq(1000, 1000)

            self.sensed_I_abc.emit(I_Aa, I_Bb, I_Cc)
            self.sensed_V_abc.emit(V_an, V_bn, V_cn)
            self.sensed_V_FFT.emit(freq_vect, fft_data)
            self.sensed_I_FFT.emit(freq_vect, fft_data) # TODO
            #self.reference_V_abc.emit(ref_a, ref_b, ref_c)  # TODO

            app.processEvents()


if __name__ == '__main__':
    
    app = QtGui.QApplication([])
    plt = Inverter()

    serial_port = None #serial.Serial('COM9', 921600)

    fft_a_graph_update = pyqtSignal(object, object)

    serial_read_worker = SerialReader(serial_port)
    #fft_a_worker = FFT()

    #fft_a_thread = QThread()
    #fft_a_worker.moveToThread(fft_a_thread)
    serial_read_thread = QThread()
    serial_read_worker.moveToThread(serial_read_thread)

    serial_read_thread.started.connect(serial_read_worker.run)

    serial_read_worker.sensed_I_abc.connect(plt.sensed_current_plot.take_sample)
    serial_read_worker.sensed_V_abc.connect(plt.sensed_voltage_plot.take_sample)
    serial_read_worker.sensed_V_FFT.connect(plt.sensed_V_FFT_plot.take_sample)
    serial_read_worker.sensed_I_FFT.connect(plt.sensed_I_FFT_plot.take_sample)

    serial_read_thread.start()

    plt.run()  # Start plotting

    sys.exit(app.exec_())


