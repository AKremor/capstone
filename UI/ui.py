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


class LiveFFTWidget(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.buffer_size = 1000
        self.timewindow = 10

        self.fft_data = np.zeros(self.buffer_size, dtype=np.float)
        self.freq_vect = np.zeros(self.buffer_size, dtype=np.float)
        
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
        self.plotter.setLabel('left', 'Amplitude', 'A')
        self.plotter.setLabel('bottom', 'Frequency', 'Hz')
        self.curve = self.plotter.plot(self.freq_vect, self.fft_data, pen=(255,0,0))
        self.lay.addWidget(self.plotter)

    def updateplot(self):

        if self.fft_data is None:
            return
        self.curve.setData(self.freq_vect, self.fft_data)
        self.fft_data = None

    def give_data(self, freq_vect, fft_data):
        self.fft_data = fft_data
        self.freq_vect = freq_vect

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

        # Create the SVM plane
        numPoints = 10
        #x = np.random.normal(size=numPoints)
        #y = np.random.normal(size=numPoints)
        #self.curve.setData(x, y)

    def set_ref_vector(self, ref_x, ref_y):
        self.ref_vector_tip_x = ref_x
        self.ref_vector_tip_y = ref_y

    def updateplot(self):
        self.ref_vector_curve.setData([0, self.ref_vector_tip_x], [0, self.ref_vector_tip_y])

class LivePlotWidget(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.buffer_size = 1000
        self.timewindow = 10
        self.x = np.linspace(-self.timewindow, 0.0, self.buffer_size)
        self.y = np.zeros(self.buffer_size, dtype=np.float)
        self.databuffer = collections.deque([0.0]*self.buffer_size, self.buffer_size)
        self.givedata_buffer = None
        
        # customize the UI
        self.initUI()

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(1)

    def initUI(self):
        self.lay = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.setLabel('left', 'Amplitude', 'A')
        self.plotter.setLabel('bottom', 'Time', 's')
        self.curve = self.plotter.plot(self.x, self.y, pen=(255,0,0))
        self.lay.addWidget(self.plotter)

    def updateplot(self):

        if self.givedata_buffer is None:
            return
        # If we haven't received data to givedata_buffer
        self.databuffer.append( self.givedata_buffer )

        self.y[:] = self.databuffer
        self.curve.setData(self.x, self.y)
        #self.app.processEvents()

    def data_wrapper(self, func, *args, **kwargs):
            def wrapped(*args, **kwargs):
                while True:
                    res = func(*args, **kwargs)
                    time.sleep(float(self._interval) / 1000)
                    self.givedata(res)

            return wrapped

    def give_data(self, data):
        self.givedata_buffer = data
        # Takes in an x,y tuple


class Live3PhasePlotWidget(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)

        self.buffer_size = 1000
        self.timewindow = 10
        self.x = np.linspace(-self.timewindow, 0.0, self.buffer_size)
        self.y_a = np.zeros(self.buffer_size, dtype=np.float)
        self.y_b = np.zeros(self.buffer_size, dtype=np.float)
        self.y_c = np.zeros(self.buffer_size, dtype=np.float)
        self.databuffer_a = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.databuffer_b = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.databuffer_c = collections.deque([0.0] * self.buffer_size, self.buffer_size)
        self.givedata_buffer = None

        # customize the UI
        self.initUI()

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(1)

    def initUI(self):
        self.lay = QVBoxLayout(self)
        self.plotter = pg.PlotWidget()
        self.plotter.showGrid(x=True, y=True)
        self.plotter.setLabel('left', 'Amplitude', 'A')
        self.plotter.setLabel('bottom', 'Time', 's')
        self.curve_a = self.plotter.plot(self.x, self.y_a, pen=(255, 0, 0))
        self.curve_b = self.plotter.plot(self.x, self.y_b, pen=(128, 255, 0))
        self.curve_c = self.plotter.plot(self.x, self.y_c, pen=(128, 0, 255))
        self.lay.addWidget(self.plotter)

    def updateplot(self):
        if self.givedata_buffer is None:
            return

        # If we haven't received data to givedata_buffer
        self.databuffer_a.append(self.givedata_buffer[0])
        self.databuffer_b.append(self.givedata_buffer[1])
        self.databuffer_c.append(self.givedata_buffer[2])

        self.y_a[:] = self.databuffer_a
        self.y_b[:] = self.databuffer_b
        self.y_c[:] = self.databuffer_c

        self.curve_a.setData(self.x, self.y_a)
        self.curve_b.setData(self.x, self.y_b)
        self.curve_c.setData(self.x, self.y_c)

    @QtCore.pyqtSlot(float, float, float)
    def take_sample(self, a, b, c):
        self.givedata_buffer = [a, b, c]


class Inverter(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.reference_current_plot = Live3PhasePlotWidget()
        self.sensed_current_plot = Live3PhasePlotWidget()

        self.initUI()

    def initUI(self):
        grid = QGridLayout()
        grid.setSpacing(10)

        self.fixedFreqSlider = QtGui.QSlider(QtCore.Qt.Horizontal)
        self.fixedFreqSlider.setMinimum(0.0001)
        self.fixedFreqSlider.setMaximum(10)
        self.fixedFreqSlider.setSingleStep(0.001)
        self.freq_entry = QLineEdit()
        self.freq_entry.setValidator(QDoubleValidator(0.001, 1000, 2))

        self.magSlider = QtGui.QSlider(QtCore.Qt.Horizontal)
        self.magSlider.setMinimum(0.001)
        self.magSlider.setMaximum(10)
        self.magSlider.setSingleStep(0.001)

        grid.addWidget(self.reference_current_plot, 1, 1, 1, 2)
        grid.addWidget(self.sensed_current_plot, 1, 3, 1, 2)

        self.current_fft = LiveFFTWidget()
        grid.addWidget(self.current_fft, 2, 1, 1, 2)

        #freq_label = QLabel("Frequency")
        #mag_label = QLabel("Magnitude")
        #freq_label.setAlignment(QtCore.Qt.AlignCenter)
        #mag_label.setAlignment(QtCore.Qt.AlignCenter)

        #grid.addWidget(freq_label, 3,1)
        #grid.addWidget(mag_label, 3,2)
        #grid.addWidget(self.fixedFreqSlider, 4, 1)
        #grid.addWidget(self.freq_entry, 5, 1)
        #grid.addWidget(self.magSlider,4,2)
        
        self.svm_plane = LiveSVMWidget()
        grid.addWidget(self.svm_plane, 6,1, 1, 2)
        self.setLayout(grid) 
        
        self.resize(900, 600)
        self.setWindowTitle('Inverter')    
    
    def run(self):
        self.show()

class SerialReader(QObject):
    reference_I_abc = pyqtSignal(float, float, float, name='reference_I_abc')
    sensed_I_abc = pyqtSignal(float, float, float, name='sensed_I_abc')

    reference_V_abc = pyqtSignal(float, float, float, name='reference_V_abc')
    sensed_V_abc = pyqtSignal(float, float, float, name='sensed_V_abc')

    def __init__(self, serial_port):
        super().__init__()
        self.serial_port = serial_port

    @pyqtSlot()
    def run(self):
        while True:
            time.sleep(0.001)
            ref_a =  math.sin(time.time() * 2 * 1 * math.pi)
            ref_b =  math.sin(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
            ref_c =  math.sin(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

            self.sensed_I_abc.emit(random.randint(1,5), random.randint(1,5), random.randint(1,5)) # TODO
            self.reference_I_abc.emit(ref_a, ref_b, ref_c)  # TODO

            self.sensed_V_abc.emit(ref_a, ref_b, ref_c)  # TODO
            self.reference_V_abc.emit(ref_a, ref_b, ref_c)  # TODO

            app.processEvents()

class FFT(QObject):
    new_fft_sample = pyqtSignal(float)

    def __init__(self, buffer_size):
        super().__init__()
        self.buffer_size = buffer_size

    @pyqtSlot()
    def run(self):
        while True:
            time.sleep(0.001)
            ref_a =  math.sin(time.time() * 2 * 1 * math.pi)
            ref_b =  math.sin(time.time() * 2 * 1 * math.pi - 2 * math.pi / 3)
            ref_c =  math.sin(time.time() * 2 * 1 * math.pi + 2 * math.pi / 3)

            self.sensed_I_abc.emit(random.randint(1,5), random.randint(1,5), random.randint(1,5)) # TODO
            self.reference_I_abc.emit(ref_a, ref_b, ref_c)  # TODO

            self.sensed_V_abc.emit(ref_a, ref_b, ref_c)  # TODO
            self.reference_V_abc.emit(ref_a, ref_b, ref_c)  # TODO

            app.processEvents()

def gen_data(parents, magnitude, frequency, max_noise, svm_plane):
    databuffer = collections.deque([0.0] * 1000, 1000)
    while True:
        time.sleep(float(10) / 1000)
        noise = random.normalvariate(0., max_noise)
        freq = frequency.value()

        new = magnitude.value()*math.sin(time.time()*2*freq *math.pi) + noise
        newb = magnitude.value()*math.sin(time.time()*2*freq *math.pi - 2 * math.pi/3) + noise
        newc = magnitude.value()*math.sin(time.time()*2*freq *math.pi + 2 * math.pi/3) + noise


        databuffer.append(new)
        fft_data = np.abs(np.fft.rfft(databuffer))
        freq_vect = np.fft.rfftfreq(1000, 1000)
        parents[1].give_data(freq_vect, fft_data)

        # calculate the gh value
        newa = new
        Vdc = 1

        g = 1 / (3 * Vdc) * (2 * newa - newb - newc)
        h = 1 / (3 * Vdc) * (-1 * newa + 2 * newb - newc)
        svm_plane.set_ref_vector(g, h)

if __name__ == '__main__':
    
    app = QtGui.QApplication([])
    plt = Inverter()

    serial_port = None#serial.Serial('COM7', 921600)

    serial_read_worker = SerialReader(serial_port)
    serial_read_thread = QThread()
    serial_read_worker.moveToThread(serial_read_thread)
    serial_read_thread.started.connect(serial_read_worker.run)
    serial_read_worker.sensed_I_abc.connect(plt.sensed_current_plot.take_sample)
    serial_read_worker.reference_I_abc.connect(plt.reference_current_plot.take_sample)
    serial_read_thread.start()

    # Define the thread
    th = threading.Thread(target=gen_data,
                   kwargs={'frequency': plt.fixedFreqSlider,
                           'magnitude': plt.magSlider,
                           'max_noise': 0.1,
                           'parents': [plt.reference_current_plot, plt.current_fft],
                           'svm_plane': plt.svm_plane})
    th.daemon = True

    # Finally when the set-up is ready, start everything
    th.start() # Start thread
    plt.run()  # Start plotting

    sys.exit(app.exec_())


