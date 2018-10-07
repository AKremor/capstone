import sys
from PyQt5.QtWidgets import (QWidget, QToolTip, 
    QPushButton, QApplication, QLabel, QGridLayout,QVBoxLayout,QHBoxLayout,QRadioButton)
from PyQt5.QtGui import QFont   
import pyqtgraph as pg
import random
import time
import numpy as np
import collections
import socket
import threading
import math
from pyqtgraph.Qt import QtGui, QtCore


class LiveFFTWidget(QtGui.QWidget):
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
        self.plotter.setLabel('bottom', 'Frequency', 'Hz')
        self.curve = self.plotter.plot(self.x, self.y, pen=(255,0,0))
        self.lay.addWidget(self.plotter)

    def updateplot(self):

        if self.givedata_buffer is None:
            return
        # If we haven't received data to givedata_buffer
        self.databuffer.append( self.givedata_buffer )

        self.y[:] = self.databuffer
        fft_data = np.abs(np.fft.rfft(self.y))
        freq_vect = np.fft.rfftfreq(1000, 1000)

        self.curve.setData(freq_vect, fft_data)

    def data_wrapper(self, func, *args, **kwargs):
            def wrapped(*args, **kwargs):
                while True:
                    res = func(*args, **kwargs)
                    time.sleep(float(self._interval) / 1000)
                    self.givedata(res)

            return wrapped

    def give_data(self, data):
        self.givedata_buffer = data
        # Takes in a y value, update to take in time TODO

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


class Inverter(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        
        self.initUI()

    def initUI(self):
        grid = QGridLayout()
        grid.setSpacing(10)

        self.fixedFreqSlider = QtGui.QSlider(QtCore.Qt.Horizontal)
        self.fixedFreqSlider.setMinimum(0.0001)
        self.fixedFreqSlider.setMaximum(10)
        self.fixedFreqSlider.setSingleStep(0.001)
        
        self.magSlider = QtGui.QSlider(QtCore.Qt.Horizontal)
        self.magSlider.setMinimum(0.001)
        self.magSlider.setMaximum(10)
        self.magSlider.setSingleStep(0.001)

        self.current_plot = LivePlotWidget()
        grid.addWidget(self.current_plot, 1, 1, 1, 2)

        self.current_fft = LiveFFTWidget()
        grid.addWidget(self.current_fft, 2, 1, 1, 2)

        freq_label = QLabel("Frequency")
        mag_label = QLabel("Magnitude")
        freq_label.setAlignment(QtCore.Qt.AlignCenter)
        mag_label.setAlignment(QtCore.Qt.AlignCenter)

        grid.addWidget(freq_label, 3,1)
        grid.addWidget(mag_label, 3,2)
        grid.addWidget(self.fixedFreqSlider,4,1)
        grid.addWidget(self.magSlider,4,2)
        
        self.svm_plane = LiveSVMWidget()
        grid.addWidget(self.svm_plane, 5,1, 1, 2)
        self.setLayout(grid) 
        
        self.resize(900, 600)
        self.setWindowTitle('Inverter')    
    
    def run(self):
        self.show()


def gen_data(parents, magnitude, frequency, max_noise, svm_plane):
    while True:
        time.sleep(float(1) / 1000)
        noise = random.normalvariate(0., max_noise)
        freq = frequency.value()
        print(freq)
        #freq = 0.1
        new = magnitude.value()*math.sin(time.time()*2*freq *math.pi) + noise
        newb = magnitude.value()*math.sin(time.time()*2*freq *math.pi - 2 * math.pi/3) + noise
        newc = magnitude.value()*math.sin(time.time()*2*freq *math.pi + 2 * math.pi/3) + noise
        for parent in parents:
            parent.give_data(new)

        # calculate the gh value
        newa = new
        Vdc = 1

        g = 1 / (3 * Vdc) * (2 * newa - newb - newc);
        h = 1 / (3 * Vdc) * (-1 * newa + 2 * newb - newc);
        svm_plane.set_ref_vector(g, h)

if __name__ == '__main__':
    
    app = QtGui.QApplication([])
    plt = Inverter()

    # Define the thread
    th = threading.Thread(target=gen_data,
                   kwargs={'frequency': plt.fixedFreqSlider,
                           'magnitude': plt.magSlider,
                           'max_noise': 0,
                           'parents': [plt.current_plot, plt.current_fft],
                           'svm_plane': plt.svm_plane})
    th.daemon = True

    # Finally when the set-up is ready, start everything
    th.start() # Start thread
    plt.run()  # Start plotting

    sys.exit(app.exec_())


