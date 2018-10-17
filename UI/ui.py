from PyQt5.QtCore import QObject, pyqtSlot
from PyQt5.QtCore import QThread
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QVBoxLayout
import pyqtgraph as pg
import time
import pdb
import numpy as np
import collections
import math
from pyqtgraph.Qt import QtGui, QtCore
import serial
import traceback
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

    def initUI(self):
        self.buffer_size = 1000
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
        self.n_plots = 0
        self.givedata_buffer = None

        self.ui_init = False
        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(100)

    def initUI(self, n_plots, legends, x_axis, y_axis):
        self.ui_init = True
        self.n_plots = n_plots
        self.buffer_size = 100
        self.curves = []
        self.legends = legends
        self.x_axis = x_axis
        self.y_axis = y_axis
        self.x = np.linspace(-10,0,self.buffer_size)
        self.databuffer = collections.deque([(0.0,) * n_plots]*self.buffer_size, self.buffer_size)
        self.givedata_buffer = None

        self.layout = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.addLegend()
        self.plotter.setLabel('left', *self.y_axis)
        self.plotter.setLabel('bottom', *self.x_axis)
        self.colours = [(255, 255, 255), (255, 0, 0), (0, 255, 0)]
        for i in range(self.n_plots):
            self.curves.append(self.plotter.plot(self.x, [x[i] for x in self.databuffer], pen=self.colours[i], name=legends[i]))
        self.layout.addWidget(self.plotter)

    def updateplot(self):

        if self.ui_init is False:
            print("WARNING UNINIT UI")
        if self.givedata_buffer is None:
            return

        for i in range(self.n_plots):
            self.databuffer.append(self.givedata_buffer)
            self.curves[i].setData(self.x, [x[i] for x in self.databuffer])

    @QtCore.pyqtSlot(float, float, float)
    def take_sample(self, a, b, c):
        self.givedata_buffer = (a, b, c)

    @QtCore.pyqtSlot(float, float)
    def take_sample_dq(self, a, b):
        self.givedata_buffer = (a, b)


class SerialWriter(QObject):

    serial_write_signal = pyqtSignal(object, object, name='serial_write_signal')

    def __init__(self, serial_port):
        super().__init__()
        self.serial_port = serial_port

    @pyqtSlot(int)
    def send_magnitude(self, magnitude):
        command_code_set_magnitude = 1
        for i in range(5):
            self.send(command_code_set_magnitude, magnitude)

    @pyqtSlot(int)
    def send_frequency(self, frequency):
        command_code_set_frequency = 2

        for i in range(5):
            self.send(command_code_set_frequency, frequency)

    @pyqtSlot()
    def send_pause(self):
        command_code_pause = 3

        for i in range(5):
            self.send(command_code_pause, 0)

    @pyqtSlot()
    def send_start(self):
        command_code_start = 4

        for i in range(5):
            self.send(command_code_start, 0)

    @pyqtSlot()
    def send_step(self):
        command_code_step = 5

        for i in range(5):
            self.send(command_code_step, 0)

    @pyqtSlot(int)
    def send_chopper_hz(self, chopper_hz):
        command_code_chopper_hz = 6

        for i in range(5):
            self.send(command_code_chopper_hz, chopper_hz)

    def send(self, command_code, value):
        print("Sending something to serial port")
        if self.serial_port is None:
            print("Serial port None on writer")
            return

        self.serial_port.write(bytes('A', 'ascii'))
        self.serial_port.write(bytes('a', 'ascii'))
        self.serial_port.write(bytearray([5]))
        self.serial_port.write(bytearray([command_code]))

        byte_list = pack('>i', 100000 * value)
        self.serial_port.write(bytearray(byte_list))

        app.processEvents()


class SerialReader(QObject):

    ref_voltage_dq_signal = pyqtSignal(float, float, name='ref_voltage_dq')
    ref_current_dq_signal = pyqtSignal(float, float, name='ref_current_dq')

    error_voltage_dq_signal = pyqtSignal(float, float, name='error_voltage_dq')
    error_current_dq_signal = pyqtSignal(float, float, name='error_current_dq')

    sensed_current_abc_signal = pyqtSignal(float, float, float, name='sensed_current_abc')
    sensed_current_dq_signal = pyqtSignal(float, float, name='sensed_current_dq')
    sensed_voltage_abc_signal = pyqtSignal(float, float, float, name='sensed_voltage_abc')
    sensed_voltage_dq_signal = pyqtSignal(float, float, name='sensed_voltage_dq')

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
            if self.serial_port is not None:
                self.serial_port.read_until(b'\x41\x61')

                # We have now achieved sync
                # Make next char include size
                length = int.from_bytes(self.serial_port.read(1), byteorder='little')
                bytes_read = self.serial_port.read(length)
                # print("Read {} bytes".format(len(bytes_read)))
                #print(bytes_read)
                try:
                    data = unpack('ffffffffffbbb', bytes_read)
                except:
                    continue

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

                I_d = 1 # TODO
                I_q = 0 # TODO These are sensed current
                V_d = 1 # TODO
                V_q = 0 # TODO These are sensed voltage

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

            self.ref_voltage_dq_signal.emit(Id_ref, Iq_ref)

            self.sensed_current_dq_signal.emit(I_d, I_q)
            self.sensed_voltage_dq_signal.emit(V_d, V_q)

            self.sensed_voltage_fft_signal.emit(freq_vect, voltage_fft_data)
            self.sensed_current_fft_signal.emit(freq_vect, current_fft_data)

            self.level_9_detect_signal.emit(level_9_detect)
            self.level_3_detect_signal.emit(level_3_detect)
            self.level_1_detect_signal.emit(level_1_detect)

            app.processEvents()

class InverterApp(QtGui.QMainWindow, design.Ui_MainWindow):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        serial_port = serial.Serial('COM11', 921600)

        self.serial_write_worker = SerialWriter(serial_port)
        self.serial_write_thread = QThread()
        self.serial_write_worker.moveToThread(self.serial_write_thread)

        self.serial_write_signal = self.serial_write_worker.serial_write_signal

        self.serial_read_worker = SerialReader(serial_port)
        self.serial_read_thread = QThread()
        self.serial_read_worker.moveToThread(self.serial_read_thread)
        self.serial_read_thread.started.connect(self.serial_read_worker.run)

        self.serial_read_worker.ref_voltage_dq_signal.connect(self.ref_voltage_dq.take_sample_dq)
        self.serial_read_worker.ref_current_dq_signal.connect(self.ref_current_dq.take_sample_dq)

        self.serial_read_worker.error_voltage_dq_signal.connect(self.error_voltage_dq.take_sample_dq)
        self.serial_read_worker.error_current_dq_signal.connect(self.error_current_dq.take_sample_dq)

        self.serial_read_worker.sensed_current_abc_signal.connect(self.sensed_current_abc.take_sample)
        self.serial_read_worker.sensed_current_dq_signal.connect(self.sensed_current_dq.take_sample_dq)
        self.serial_read_worker.sensed_voltage_abc_signal.connect(self.sensed_voltage_abc.take_sample)
        self.serial_read_worker.sensed_voltage_dq_signal.connect(self.sensed_voltage_dq.take_sample_dq)

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
        self.chopper_hz_slider.valueChanged.connect(self.serial_write_worker.send_chopper_hz)
        self.chopper_hz_slider.valueChanged.connect(self.chopper_hz_number.display)

        self.start_button.clicked.connect(self.serial_write_worker.send_start)
        self.pause_button.clicked.connect(self.serial_write_worker.send_pause)
        self.next_state_button.clicked.connect(self.serial_write_worker.send_step)

        self.serial_read_thread.start()
        self.serial_write_thread.start()

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


