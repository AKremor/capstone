from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtCore import QThread
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QVBoxLayout
import pyqtgraph as pg
import time
import numpy as np
import collections
import math
from pyqtgraph.Qt import QtGui, QtCore
import serial

import design

import sys
from struct import *


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
        self.plotter.addLegend()
        self.plotter.setLabel('left', *self.y_axis)
        self.plotter.setLabel('bottom', *self.x_axis)
        colours = [(255, 255, 255), (255, 0, 0), (0, 255, 0)]
        for i in range(self.n_plots):
            self.curves[i] = self.plotter.plot(self.x, self.y[i], pen=colours[i], name=legends[i])
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


class SerialWriter(QObject):

    serial_write_signal = pyqtSignal(object, object, name='serial_write_signal')

    def __init__(self, serial_port):
        super().__init__()
        self.serial_port = serial_port

    @pyqtSlot(int)
    def send_magnitude(self, magnitude):
        self.send(command_code_set_magnitude, magnitude)

    @pyqtSlot(int)
    def send_frequency(self, frequency):
        self.send(command_code_set_frequency, frequency)

    def send(self, command_code, value):
        if self.serial_port is None:
            print("Serial port None on writer")
            return

        self.serial_port.write('A')
        self.serial_port.write('a')
        self.serial_port.write(len(5))
        self.serial_port.write(command_code)

        byte_list = pack('f', value)

        for byte in byte_list:
            self.serial_port.write(byte)

        app.processEvents()


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

    level_9_detect_signal = pyqtSignal(bool, name='level_9_detect')
    level_3_detect_signal = pyqtSignal(bool, name='level_3_detect')
    level_1_detect_signal = pyqtSignal(bool, name='level_1_detect')

    def __init__(self, serial_port, buffer_size = 1000):
        super().__init__()
        self.buffer_size = buffer_size
        self.sensed_voltage_sample_buffer = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.sensed_current_sample_buffer = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.serial_port = serial_port

    @pyqtSlot()
    def run(self):
        while True:
            time.sleep(0.01)
            if self.serial_port is not None:
                self.serial_port.read_until(b'\x41\x61')

                # We have now achieved sync
                # Make next char include size
                length = int.from_bytes(self.serial_port.read(1), byteorder='little')
                data = unpack('ffffffffff', self.serial_port.read(length))

                # print("Read {} bytes".format(len(data)))
                # print(data)

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

                level_9_detect = data[10]
                level_3_detect = data[11]
                level_1_detect = data[12]

            else:
                # If the serial port crashes or does not exist use fake data
                I_magnitude = 1
                V_magnitude = 10

                I_Aa = I_magnitude * math.sin(time.time() * 2 * 1 * math.pi)
                I_Bb = I_magnitude * math.sin(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
                I_Cc = I_magnitude * math.sin(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

                V_an = V_magnitude * math.cos(time.time() * 2 * 1 * math.pi)
                V_bn = V_magnitude * math.cos(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
                V_cn = V_magnitude * math.cos(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

                I_d = I_magnitude
                I_q = 0
                V_d = V_magnitude * 0
                V_q = 0

                level_9_detect = 1
                level_3_detect = 0
                level_1_detect = 1

            self.sensed_voltage_sample_buffer.append(V_an)
            self.sensed_current_sample_buffer.append(I_Aa)

            voltage_fft_data = np.abs(np.fft.rfft(self.sensed_voltage_sample_buffer))
            current_fft_data = np.abs(np.fft.rfft(self.sensed_current_sample_buffer))
            freq_vect = np.fft.rfftfreq(1000, 1000)

            self.sensed_current_abc_signal.emit(I_Aa, I_Bb, I_Cc)
            self.sensed_voltage_abc_signal.emit(V_an, V_bn, V_cn)

            # TODO Seem faulty?
            #self.sensed_current_dq_signal.emit(I_d, I_q, 0)
            #self.sensed_voltage_dq_signal.emit(V_d, V_q, 0)

            self.sensed_voltage_fft_signal.emit(freq_vect, voltage_fft_data)
            self.sensed_current_fft_signal.emit(freq_vect, current_fft_data)

            self.level_9_detect_signal.emit(level_9_detect)
            self.level_3_detect_signal.emit(level_3_detect)
            self.level_1_detect_signal.emit(level_1_detect)

            app.processEvents()


command_code_set_magnitude = 1
command_code_set_frequency = 2

class InverterApp(QtGui.QMainWindow, design.Ui_MainWindow):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        serial_port = None  # serial.Serial('COM9', 921600)

        self.serial_write_worker = SerialWriter(serial_port)
        self.serial_write_thread = QThread()
        self.serial_write_worker.moveToThread(self.serial_write_thread)

        self.serial_write_signal = self.serial_write_worker.serial_write_signal

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

        self.ref_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Voltage', 'V'))
        self.ref_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))

        self.error_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Voltage', 'V'))
        self.error_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))

        self.sensed_current_abc.initUI(3, ['a', 'b', 'c'], ('Time', 's'), ('Current', 'A'))
        self.sensed_current_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Current', 'A'))
        self.sensed_voltage_abc.initUI(3, ['a', 'b', 'c'], ('Time', 's'), ('Voltage', 'V'))
        self.sensed_voltage_dq.initUI(2, ['d', 'q'], ('Time', 's'), ('Voltage', 'V'))

        self.sensed_voltage_fft.initUI()
        self.sensed_current_fft.initUI()

        self.serial_read_worker.level_9_detect_signal.connect(self.update_level_9_detect)
        self.serial_read_worker.level_3_detect_signal.connect(self.update_level_3_detect)
        self.serial_read_worker.level_1_detect_signal.connect(self.update_level_1_detect)

        self.magnitude_slider.valueChanged.connect(self.magnitude_number.display)
        self.magnitude_slider.valueChanged.connect(self.serial_write_worker.send_magnitude)
        self.frequency_slider.valueChanged.connect(self.frequency_number.display)
        self.frequency_slider.valueChanged.connect(self.serial_write_worker.send_frequency)


        self.serial_read_thread.start()

    @QtCore.pyqtSlot(bool)
    def update_level_9_detect(self, value):
        self.level_9_detect.setChecked(value)

    @QtCore.pyqtSlot(bool)
    def update_level_3_detect(self, value):
        self.level_3_detect.setChecked(value)

    @QtCore.pyqtSlot(bool)
    def update_level_1_detect(self, value):
        self.level_1_detect.setChecked(value)

    def run(self):
        self.show()


if __name__ == '__main__':
    
    app = QtGui.QApplication([])
    plt = InverterApp()

    plt.run()

    sys.exit(app.exec_())


