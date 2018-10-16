# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1026, 644)
        self.centralWidget = QtWidgets.QWidget(MainWindow)
        self.centralWidget.setObjectName("centralWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.centralWidget)
        self.gridLayout.setContentsMargins(11, 11, 11, 11)
        self.gridLayout.setSpacing(6)
        self.gridLayout.setObjectName("gridLayout")
        self.system_status_group_box = QtWidgets.QGroupBox(self.centralWidget)
        self.system_status_group_box.setObjectName("system_status_group_box")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.system_status_group_box)
        self.verticalLayout.setContentsMargins(11, 11, 11, 11)
        self.verticalLayout.setSpacing(6)
        self.verticalLayout.setObjectName("verticalLayout")
        self.sensed_magnitude = QtWidgets.QLCDNumber(self.system_status_group_box)
        self.sensed_magnitude.setObjectName("sensed_magnitude")
        self.verticalLayout.addWidget(self.sensed_magnitude)
        self.sensed_magnitude_label = QtWidgets.QLabel(self.system_status_group_box)
        self.sensed_magnitude_label.setObjectName("sensed_magnitude_label")
        self.verticalLayout.addWidget(self.sensed_magnitude_label)
        self.sensed_frequency = QtWidgets.QLCDNumber(self.system_status_group_box)
        self.sensed_frequency.setObjectName("sensed_frequency")
        self.verticalLayout.addWidget(self.sensed_frequency)
        self.sensed_frequency_label = QtWidgets.QLabel(self.system_status_group_box)
        self.sensed_frequency_label.setObjectName("sensed_frequency_label")
        self.verticalLayout.addWidget(self.sensed_frequency_label)
        self.level_9_detect = QtWidgets.QCheckBox(self.system_status_group_box)
        self.level_9_detect.setEnabled(False)
        self.level_9_detect.setCheckable(True)
        self.level_9_detect.setObjectName("level_9_detect")
        self.verticalLayout.addWidget(self.level_9_detect)
        self.level_3_detect = QtWidgets.QCheckBox(self.system_status_group_box)
        self.level_3_detect.setEnabled(False)
        self.level_3_detect.setCheckable(True)
        self.level_3_detect.setObjectName("level_3_detect")
        self.verticalLayout.addWidget(self.level_3_detect)
        self.level_1_detect = QtWidgets.QCheckBox(self.system_status_group_box)
        self.level_1_detect.setEnabled(False)
        self.level_1_detect.setCheckable(True)
        self.level_1_detect.setObjectName("level_1_detect")
        self.verticalLayout.addWidget(self.level_1_detect)
        self.gridLayout.addWidget(self.system_status_group_box, 1, 0, 1, 1)
        self.system_control_group_box = QtWidgets.QGroupBox(self.centralWidget)
        self.system_control_group_box.setObjectName("system_control_group_box")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.system_control_group_box)
        self.verticalLayout_2.setContentsMargins(11, 11, 11, 11)
        self.verticalLayout_2.setSpacing(6)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.start_button = QtWidgets.QPushButton(self.system_control_group_box)
        self.start_button.setObjectName("start_button")
        self.verticalLayout_2.addWidget(self.start_button)
        self.pause_button = QtWidgets.QPushButton(self.system_control_group_box)
        self.pause_button.setObjectName("pause_button")
        self.verticalLayout_2.addWidget(self.pause_button)
        self.next_state_button = QtWidgets.QPushButton(self.system_control_group_box)
        self.next_state_button.setObjectName("next_state_button")
        self.verticalLayout_2.addWidget(self.next_state_button)
        self.stop_button = QtWidgets.QPushButton(self.system_control_group_box)
        self.stop_button.setObjectName("stop_button")
        self.verticalLayout_2.addWidget(self.stop_button)
        self.mangitude_slider_label = QtWidgets.QLabel(self.system_control_group_box)
        self.mangitude_slider_label.setObjectName("mangitude_slider_label")
        self.verticalLayout_2.addWidget(self.mangitude_slider_label)
        self.magnitude_slider = QtWidgets.QSlider(self.system_control_group_box)
        self.magnitude_slider.setMaximum(20)
        self.magnitude_slider.setOrientation(QtCore.Qt.Horizontal)
        self.magnitude_slider.setObjectName("magnitude_slider")
        self.verticalLayout_2.addWidget(self.magnitude_slider)
        self.magnitude_number = QtWidgets.QLCDNumber(self.system_control_group_box)
        self.magnitude_number.setObjectName("magnitude_number")
        self.verticalLayout_2.addWidget(self.magnitude_number)
        self.frequency_slider_label = QtWidgets.QLabel(self.system_control_group_box)
        self.frequency_slider_label.setObjectName("frequency_slider_label")
        self.verticalLayout_2.addWidget(self.frequency_slider_label)
        self.frequency_slider = QtWidgets.QSlider(self.system_control_group_box)
        self.frequency_slider.setMaximum(500)
        self.frequency_slider.setSingleStep(1)
        self.frequency_slider.setOrientation(QtCore.Qt.Horizontal)
        self.frequency_slider.setObjectName("frequency_slider")
        self.verticalLayout_2.addWidget(self.frequency_slider)
        self.frequency_number = QtWidgets.QLCDNumber(self.system_control_group_box)
        self.frequency_number.setObjectName("frequency_number")
        self.verticalLayout_2.addWidget(self.frequency_number)
        self.gridLayout.addWidget(self.system_control_group_box, 0, 0, 1, 1)
        self.reference_group_box = QtWidgets.QGroupBox(self.centralWidget)
        self.reference_group_box.setObjectName("reference_group_box")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.reference_group_box)
        self.gridLayout_2.setContentsMargins(11, 11, 11, 11)
        self.gridLayout_2.setSpacing(6)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.label = QtWidgets.QLabel(self.reference_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label.setFont(font)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")
        self.gridLayout_2.addWidget(self.label, 0, 0, 1, 1)
        self.label_2 = QtWidgets.QLabel(self.reference_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label_2.setFont(font)
        self.label_2.setAlignment(QtCore.Qt.AlignCenter)
        self.label_2.setObjectName("label_2")
        self.gridLayout_2.addWidget(self.label_2, 0, 1, 1, 1)
        self.ref_voltage_dq = LivePlotWidget(self.reference_group_box)
        self.ref_voltage_dq.setObjectName("ref_voltage_dq")
        self.gridLayout_2.addWidget(self.ref_voltage_dq, 2, 0, 1, 1)
        self.ref_current_dq = LivePlotWidget(self.reference_group_box)
        self.ref_current_dq.setObjectName("ref_current_dq")
        self.gridLayout_2.addWidget(self.ref_current_dq, 2, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(self.reference_group_box)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_3.setFont(font)
        self.label_3.setAlignment(QtCore.Qt.AlignCenter)
        self.label_3.setObjectName("label_3")
        self.gridLayout_2.addWidget(self.label_3, 1, 0, 1, 2)
        self.gridLayout.addWidget(self.reference_group_box, 0, 1, 1, 1)
        self.control_group_box = QtWidgets.QGroupBox(self.centralWidget)
        self.control_group_box.setObjectName("control_group_box")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.control_group_box)
        self.gridLayout_3.setContentsMargins(11, 11, 11, 11)
        self.gridLayout_3.setSpacing(6)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.label_8 = QtWidgets.QLabel(self.control_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label_8.setFont(font)
        self.label_8.setAlignment(QtCore.Qt.AlignCenter)
        self.label_8.setObjectName("label_8")
        self.gridLayout_3.addWidget(self.label_8, 0, 0, 1, 1)
        self.label_5 = QtWidgets.QLabel(self.control_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label_5.setFont(font)
        self.label_5.setAlignment(QtCore.Qt.AlignCenter)
        self.label_5.setObjectName("label_5")
        self.gridLayout_3.addWidget(self.label_5, 0, 1, 1, 1)
        self.label_6 = QtWidgets.QLabel(self.control_group_box)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_6.setFont(font)
        self.label_6.setAlignment(QtCore.Qt.AlignCenter)
        self.label_6.setObjectName("label_6")
        self.gridLayout_3.addWidget(self.label_6, 1, 0, 1, 2)
        self.error_voltage_dq = LivePlotWidget(self.control_group_box)
        self.error_voltage_dq.setObjectName("error_voltage_dq")
        self.gridLayout_3.addWidget(self.error_voltage_dq, 2, 0, 1, 1)
        self.error_current_dq = LivePlotWidget(self.control_group_box)
        self.error_current_dq.setObjectName("error_current_dq")
        self.gridLayout_3.addWidget(self.error_current_dq, 2, 1, 1, 1)
        self.gridLayout.addWidget(self.control_group_box, 1, 1, 1, 1)
        self.output_group_box = QtWidgets.QGroupBox(self.centralWidget)
        self.output_group_box.setObjectName("output_group_box")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.output_group_box)
        self.gridLayout_4.setContentsMargins(11, 11, 11, 11)
        self.gridLayout_4.setSpacing(6)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.label_12 = QtWidgets.QLabel(self.output_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label_12.setFont(font)
        self.label_12.setAlignment(QtCore.Qt.AlignCenter)
        self.label_12.setObjectName("label_12")
        self.gridLayout_4.addWidget(self.label_12, 0, 0, 1, 1)
        self.label_9 = QtWidgets.QLabel(self.output_group_box)
        font = QtGui.QFont()
        font.setPointSize(16)
        self.label_9.setFont(font)
        self.label_9.setAlignment(QtCore.Qt.AlignCenter)
        self.label_9.setObjectName("label_9")
        self.gridLayout_4.addWidget(self.label_9, 0, 1, 1, 1)
        self.label_11 = QtWidgets.QLabel(self.output_group_box)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_11.setFont(font)
        self.label_11.setAlignment(QtCore.Qt.AlignCenter)
        self.label_11.setObjectName("label_11")
        self.gridLayout_4.addWidget(self.label_11, 1, 0, 1, 2)
        self.sensed_voltage_dq = LivePlotWidget(self.output_group_box)
        self.sensed_voltage_dq.setObjectName("sensed_voltage_dq")
        self.gridLayout_4.addWidget(self.sensed_voltage_dq, 2, 0, 1, 1)
        self.sensed_current_dq = LivePlotWidget(self.output_group_box)
        self.sensed_current_dq.setObjectName("sensed_current_dq")
        self.gridLayout_4.addWidget(self.sensed_current_dq, 2, 1, 1, 1)
        self.label_14 = QtWidgets.QLabel(self.output_group_box)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_14.setFont(font)
        self.label_14.setAlignment(QtCore.Qt.AlignCenter)
        self.label_14.setObjectName("label_14")
        self.gridLayout_4.addWidget(self.label_14, 3, 0, 1, 2)
        self.sensed_voltage_abc = LivePlotWidget(self.output_group_box)
        self.sensed_voltage_abc.setObjectName("sensed_voltage_abc")
        self.gridLayout_4.addWidget(self.sensed_voltage_abc, 4, 0, 1, 1)
        self.sensed_current_abc = LivePlotWidget(self.output_group_box)
        self.sensed_current_abc.setObjectName("sensed_current_abc")
        self.gridLayout_4.addWidget(self.sensed_current_abc, 4, 1, 1, 1)
        self.label_16 = QtWidgets.QLabel(self.output_group_box)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_16.setFont(font)
        self.label_16.setAlignment(QtCore.Qt.AlignCenter)
        self.label_16.setObjectName("label_16")
        self.gridLayout_4.addWidget(self.label_16, 5, 0, 1, 2)
        self.sensed_voltage_fft = LiveFFTWidget(self.output_group_box)
        self.sensed_voltage_fft.setObjectName("sensed_voltage_fft")
        self.gridLayout_4.addWidget(self.sensed_voltage_fft, 6, 0, 1, 1)
        self.sensed_current_fft = LiveFFTWidget(self.output_group_box)
        self.sensed_current_fft.setObjectName("sensed_current_fft")
        self.gridLayout_4.addWidget(self.sensed_current_fft, 6, 1, 1, 1)
        self.gridLayout.addWidget(self.output_group_box, 0, 2, 2, 1)
        MainWindow.setCentralWidget(self.centralWidget)
        self.menuBar = QtWidgets.QMenuBar(MainWindow)
        self.menuBar.setGeometry(QtCore.QRect(0, 0, 1026, 21))
        self.menuBar.setObjectName("menuBar")
        MainWindow.setMenuBar(self.menuBar)
        self.mainToolBar = QtWidgets.QToolBar(MainWindow)
        self.mainToolBar.setObjectName("mainToolBar")
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.mainToolBar)
        self.statusBar = QtWidgets.QStatusBar(MainWindow)
        self.statusBar.setObjectName("statusBar")
        MainWindow.setStatusBar(self.statusBar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.system_status_group_box.setTitle(_translate("MainWindow", "System Status"))
        self.sensed_magnitude_label.setText(_translate("MainWindow", "Sensed Magnitude"))
        self.sensed_frequency_label.setText(_translate("MainWindow", "Sensed Frequency"))
        self.level_9_detect.setText(_translate("MainWindow", "9V level detected"))
        self.level_3_detect.setText(_translate("MainWindow", "3V level detected"))
        self.level_1_detect.setText(_translate("MainWindow", "1V level detected"))
        self.system_control_group_box.setTitle(_translate("MainWindow", "System Control"))
        self.start_button.setText(_translate("MainWindow", "Start"))
        self.pause_button.setText(_translate("MainWindow", "Pause"))
        self.next_state_button.setText(_translate("MainWindow", "Next State"))
        self.stop_button.setText(_translate("MainWindow", "Stop"))
        self.mangitude_slider_label.setText(_translate("MainWindow", "Magnitude (Vpp)"))
        self.frequency_slider_label.setText(_translate("MainWindow", "Frequency (Hz)"))
        self.reference_group_box.setTitle(_translate("MainWindow", "Reference"))
        self.label.setText(_translate("MainWindow", "Voltage"))
        self.label_2.setText(_translate("MainWindow", "Current"))
        self.label_3.setText(_translate("MainWindow", "DQ"))
        self.control_group_box.setTitle(_translate("MainWindow", "Control"))
        self.label_8.setText(_translate("MainWindow", "Voltage"))
        self.label_5.setText(_translate("MainWindow", "Current"))
        self.label_6.setText(_translate("MainWindow", "DQ error"))
        self.output_group_box.setTitle(_translate("MainWindow", "Output"))
        self.label_12.setText(_translate("MainWindow", "Voltage"))
        self.label_9.setText(_translate("MainWindow", "Current"))
        self.label_11.setText(_translate("MainWindow", "DQ"))
        self.label_14.setText(_translate("MainWindow", "ABC"))
        self.label_16.setText(_translate("MainWindow", "FFT"))

from ui import LiveFFTWidget, LivePlotWidget
