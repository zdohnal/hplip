# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui4/settingsdialog_base.ui'
#
# Created: Tue Feb 17 11:36:14 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_SettingsDialog_base(object):
    def setupUi(self, SettingsDialog_base):
        SettingsDialog_base.setObjectName("SettingsDialog_base")
        SettingsDialog_base.resize(QtCore.QSize(QtCore.QRect(0,0,500,540).size()).expandedTo(SettingsDialog_base.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(SettingsDialog_base)
        self.gridlayout.setObjectName("gridlayout")

        self.TabWidget = QtGui.QTabWidget(SettingsDialog_base)
        self.TabWidget.setObjectName("TabWidget")

        self.AutoRefresh = QtGui.QWidget()
        self.AutoRefresh.setObjectName("AutoRefresh")

        self.gridlayout1 = QtGui.QGridLayout(self.AutoRefresh)
        self.gridlayout1.setObjectName("gridlayout1")

        self.textLabel3_2_2 = QtGui.QLabel(self.AutoRefresh)
        self.textLabel3_2_2.setWordWrap(False)
        self.textLabel3_2_2.setObjectName("textLabel3_2_2")
        self.gridlayout1.addWidget(self.textLabel3_2_2,0,0,1,1)

        self.line1_2_2 = QtGui.QFrame(self.AutoRefresh)
        self.line1_2_2.setFrameShape(QtGui.QFrame.HLine)
        self.line1_2_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line1_2_2.setObjectName("line1_2_2")
        self.gridlayout1.addWidget(self.line1_2_2,1,0,1,1)

        self.AutoRefreshCheckBox = QtGui.QCheckBox(self.AutoRefresh)
        self.AutoRefreshCheckBox.setObjectName("AutoRefreshCheckBox")
        self.gridlayout1.addWidget(self.AutoRefreshCheckBox,2,0,1,1)

        self.RefreshRateGroupBox = QtGui.QGroupBox(self.AutoRefresh)
        self.RefreshRateGroupBox.setEnabled(True)
        self.RefreshRateGroupBox.setObjectName("RefreshRateGroupBox")

        self.gridlayout2 = QtGui.QGridLayout(self.RefreshRateGroupBox)
        self.gridlayout2.setObjectName("gridlayout2")

        self.textLabel1_4 = QtGui.QLabel(self.RefreshRateGroupBox)
        self.textLabel1_4.setWordWrap(False)
        self.textLabel1_4.setObjectName("textLabel1_4")
        self.gridlayout2.addWidget(self.textLabel1_4,0,0,1,1)

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        self.AutoRefreshRateSpinBox = QtGui.QSpinBox(self.RefreshRateGroupBox)
        self.AutoRefreshRateSpinBox.setEnabled(False)
        self.AutoRefreshRateSpinBox.setWrapping(True)
        self.AutoRefreshRateSpinBox.setButtonSymbols(QtGui.QAbstractSpinBox.PlusMinus)
        self.AutoRefreshRateSpinBox.setMinimum(10)
        self.AutoRefreshRateSpinBox.setMaximum(300)
        self.AutoRefreshRateSpinBox.setProperty("value",QtCore.QVariant(30))
        self.AutoRefreshRateSpinBox.setObjectName("AutoRefreshRateSpinBox")
        self.hboxlayout.addWidget(self.AutoRefreshRateSpinBox)
        self.gridlayout2.addLayout(self.hboxlayout,0,1,1,1)

        spacerItem = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout2.addItem(spacerItem,0,2,1,1)
        self.gridlayout1.addWidget(self.RefreshRateGroupBox,3,0,1,1)

        self.RefreshTypeGroupBox = QtGui.QGroupBox(self.AutoRefresh)
        self.RefreshTypeGroupBox.setObjectName("RefreshTypeGroupBox")

        self.gridlayout3 = QtGui.QGridLayout(self.RefreshTypeGroupBox)
        self.gridlayout3.setObjectName("gridlayout3")

        self.RefreshCurrentRadioButton = QtGui.QRadioButton(self.RefreshTypeGroupBox)
        self.RefreshCurrentRadioButton.setEnabled(False)
        self.RefreshCurrentRadioButton.setChecked(True)
        self.RefreshCurrentRadioButton.setObjectName("RefreshCurrentRadioButton")
        self.gridlayout3.addWidget(self.RefreshCurrentRadioButton,0,0,1,1)

        self.RefreshAllRadioButton = QtGui.QRadioButton(self.RefreshTypeGroupBox)
        self.RefreshAllRadioButton.setEnabled(False)
        self.RefreshAllRadioButton.setObjectName("RefreshAllRadioButton")
        self.gridlayout3.addWidget(self.RefreshAllRadioButton,1,0,1,1)
        self.gridlayout1.addWidget(self.RefreshTypeGroupBox,4,0,1,1)

        spacerItem1 = QtGui.QSpacerItem(446,41,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout1.addItem(spacerItem1,5,0,1,1)
        self.TabWidget.addTab(self.AutoRefresh,"")

        self.SystemTray = QtGui.QWidget()
        self.SystemTray.setObjectName("SystemTray")

        self.gridlayout4 = QtGui.QGridLayout(self.SystemTray)
        self.gridlayout4.setObjectName("gridlayout4")

        self.textLabel3_2_3 = QtGui.QLabel(self.SystemTray)
        self.textLabel3_2_3.setWordWrap(False)
        self.textLabel3_2_3.setObjectName("textLabel3_2_3")
        self.gridlayout4.addWidget(self.textLabel3_2_3,0,0,1,1)

        self.line1_2_3 = QtGui.QFrame(self.SystemTray)
        self.line1_2_3.setFrameShape(QtGui.QFrame.HLine)
        self.line1_2_3.setFrameShadow(QtGui.QFrame.Sunken)
        self.line1_2_3.setObjectName("line1_2_3")
        self.gridlayout4.addWidget(self.line1_2_3,1,0,1,1)

        self.SystemTraySettings = SystrayFrame(self.SystemTray)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.SystemTraySettings.sizePolicy().hasHeightForWidth())
        self.SystemTraySettings.setSizePolicy(sizePolicy)
        self.SystemTraySettings.setFrameShadow(QtGui.QFrame.Raised)
        self.SystemTraySettings.setObjectName("SystemTraySettings")
        self.gridlayout4.addWidget(self.SystemTraySettings,2,0,1,1)
        self.TabWidget.addTab(self.SystemTray,"")

        self.Commands = QtGui.QWidget()
        self.Commands.setObjectName("Commands")

        self.gridlayout5 = QtGui.QGridLayout(self.Commands)
        self.gridlayout5.setObjectName("gridlayout5")

        self.textLabel3_2_2_2 = QtGui.QLabel(self.Commands)
        self.textLabel3_2_2_2.setWordWrap(False)
        self.textLabel3_2_2_2.setObjectName("textLabel3_2_2_2")
        self.gridlayout5.addWidget(self.textLabel3_2_2_2,0,0,1,2)

        self.line1_2_2_3 = QtGui.QFrame(self.Commands)
        self.line1_2_2_3.setFrameShape(QtGui.QFrame.HLine)
        self.line1_2_2_3.setFrameShadow(QtGui.QFrame.Sunken)
        self.line1_2_2_3.setObjectName("line1_2_2_3")
        self.gridlayout5.addWidget(self.line1_2_2_3,1,0,1,2)

        self.groupBox_3 = QtGui.QGroupBox(self.Commands)
        self.groupBox_3.setObjectName("groupBox_3")

        self.gridlayout6 = QtGui.QGridLayout(self.groupBox_3)
        self.gridlayout6.setObjectName("gridlayout6")

        self.textLabel1_2 = QtGui.QLabel(self.groupBox_3)
        self.textLabel1_2.setEnabled(False)
        self.textLabel1_2.setWordWrap(False)
        self.textLabel1_2.setObjectName("textLabel1_2")
        self.gridlayout6.addWidget(self.textLabel1_2,0,0,1,1)

        self.PrintCommandLineEdit = QtGui.QLineEdit(self.groupBox_3)
        self.PrintCommandLineEdit.setEnabled(False)
        self.PrintCommandLineEdit.setObjectName("PrintCommandLineEdit")
        self.gridlayout6.addWidget(self.PrintCommandLineEdit,1,0,1,1)

        self.textLabel2_2 = QtGui.QLabel(self.groupBox_3)
        self.textLabel2_2.setWordWrap(False)
        self.textLabel2_2.setObjectName("textLabel2_2")
        self.gridlayout6.addWidget(self.textLabel2_2,2,0,1,1)

        self.ScanCommandLineEdit = QtGui.QLineEdit(self.groupBox_3)
        self.ScanCommandLineEdit.setObjectName("ScanCommandLineEdit")
        self.gridlayout6.addWidget(self.ScanCommandLineEdit,3,0,1,1)

        self.textLabel3_3 = QtGui.QLabel(self.groupBox_3)
        self.textLabel3_3.setEnabled(False)
        self.textLabel3_3.setWordWrap(False)
        self.textLabel3_3.setObjectName("textLabel3_3")
        self.gridlayout6.addWidget(self.textLabel3_3,4,0,1,1)

        self.SendFaxCommandLineEdit = QtGui.QLineEdit(self.groupBox_3)
        self.SendFaxCommandLineEdit.setEnabled(False)
        self.SendFaxCommandLineEdit.setObjectName("SendFaxCommandLineEdit")
        self.gridlayout6.addWidget(self.SendFaxCommandLineEdit,5,0,1,1)

        self.textLabel4 = QtGui.QLabel(self.groupBox_3)
        self.textLabel4.setEnabled(False)
        self.textLabel4.setWordWrap(False)
        self.textLabel4.setObjectName("textLabel4")
        self.gridlayout6.addWidget(self.textLabel4,6,0,1,1)

        self.AccessPCardCommandLineEdit = QtGui.QLineEdit(self.groupBox_3)
        self.AccessPCardCommandLineEdit.setEnabled(False)
        self.AccessPCardCommandLineEdit.setObjectName("AccessPCardCommandLineEdit")
        self.gridlayout6.addWidget(self.AccessPCardCommandLineEdit,7,0,1,1)

        self.textLabel5 = QtGui.QLabel(self.groupBox_3)
        self.textLabel5.setEnabled(False)
        self.textLabel5.setWordWrap(False)
        self.textLabel5.setObjectName("textLabel5")
        self.gridlayout6.addWidget(self.textLabel5,8,0,1,1)

        self.MakeCopiesCommandLineEdit = QtGui.QLineEdit(self.groupBox_3)
        self.MakeCopiesCommandLineEdit.setEnabled(False)
        self.MakeCopiesCommandLineEdit.setObjectName("MakeCopiesCommandLineEdit")
        self.gridlayout6.addWidget(self.MakeCopiesCommandLineEdit,9,0,1,1)
        self.gridlayout5.addWidget(self.groupBox_3,2,0,1,2)

        spacerItem2 = QtGui.QSpacerItem(20,60,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout5.addItem(spacerItem2,3,1,1,1)

        self.SetDefaultsButton = QtGui.QPushButton(self.Commands)
        self.SetDefaultsButton.setEnabled(True)
        self.SetDefaultsButton.setObjectName("SetDefaultsButton")
        self.gridlayout5.addWidget(self.SetDefaultsButton,4,0,1,1)

        spacerItem3 = QtGui.QSpacerItem(20,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout5.addItem(spacerItem3,4,1,1,1)
        self.TabWidget.addTab(self.Commands,"")
        self.gridlayout.addWidget(self.TabWidget,0,0,1,2)

        spacerItem4 = QtGui.QSpacerItem(301,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout.addItem(spacerItem4,1,0,1,1)

        self.StdButtons = QtGui.QDialogButtonBox(SettingsDialog_base)
        self.StdButtons.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.StdButtons.setCenterButtons(False)
        self.StdButtons.setObjectName("StdButtons")
        self.gridlayout.addWidget(self.StdButtons,1,1,1,1)
        self.textLabel1_4.setBuddy(self.AutoRefreshRateSpinBox)
        self.textLabel1_2.setBuddy(self.PrintCommandLineEdit)
        self.textLabel2_2.setBuddy(self.ScanCommandLineEdit)
        self.textLabel3_3.setBuddy(self.SendFaxCommandLineEdit)
        self.textLabel4.setBuddy(self.AccessPCardCommandLineEdit)
        self.textLabel5.setBuddy(self.MakeCopiesCommandLineEdit)

        self.retranslateUi(SettingsDialog_base)
        self.TabWidget.setCurrentIndex(2)
        QtCore.QObject.connect(self.AutoRefreshCheckBox,QtCore.SIGNAL("toggled(bool)"),self.AutoRefreshRateSpinBox.setEnabled)
        QtCore.QObject.connect(self.AutoRefreshCheckBox,QtCore.SIGNAL("toggled(bool)"),self.RefreshAllRadioButton.setEnabled)
        QtCore.QObject.connect(self.AutoRefreshCheckBox,QtCore.SIGNAL("toggled(bool)"),self.RefreshCurrentRadioButton.setEnabled)
        QtCore.QObject.connect(self.StdButtons,QtCore.SIGNAL("accepted()"),SettingsDialog_base.accept)
        QtCore.QObject.connect(self.StdButtons,QtCore.SIGNAL("rejected()"),SettingsDialog_base.reject)
        QtCore.QMetaObject.connectSlotsByName(SettingsDialog_base)
        SettingsDialog_base.setTabOrder(self.TabWidget,self.AutoRefreshCheckBox)
        SettingsDialog_base.setTabOrder(self.AutoRefreshCheckBox,self.AutoRefreshRateSpinBox)
        SettingsDialog_base.setTabOrder(self.AutoRefreshRateSpinBox,self.RefreshCurrentRadioButton)
        SettingsDialog_base.setTabOrder(self.RefreshCurrentRadioButton,self.RefreshAllRadioButton)
        SettingsDialog_base.setTabOrder(self.RefreshAllRadioButton,self.PrintCommandLineEdit)
        SettingsDialog_base.setTabOrder(self.PrintCommandLineEdit,self.ScanCommandLineEdit)
        SettingsDialog_base.setTabOrder(self.ScanCommandLineEdit,self.SendFaxCommandLineEdit)
        SettingsDialog_base.setTabOrder(self.SendFaxCommandLineEdit,self.AccessPCardCommandLineEdit)
        SettingsDialog_base.setTabOrder(self.AccessPCardCommandLineEdit,self.MakeCopiesCommandLineEdit)
        SettingsDialog_base.setTabOrder(self.MakeCopiesCommandLineEdit,self.SetDefaultsButton)
        SettingsDialog_base.setTabOrder(self.SetDefaultsButton,self.StdButtons)

    def retranslateUi(self, SettingsDialog_base):
        SettingsDialog_base.setWindowTitle(QtGui.QApplication.translate("SettingsDialog_base", "HP Device Manager - Settings", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel3_2_2.setText(QtGui.QApplication.translate("SettingsDialog_base", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'DejaVu Sans\'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Configure if and when devices are automatically refreshed</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.AutoRefreshCheckBox.setText(QtGui.QApplication.translate("SettingsDialog_base", "&Enable device auto refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshRateGroupBox.setTitle(QtGui.QApplication.translate("SettingsDialog_base", "Refresh Interval", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel1_4.setText(QtGui.QApplication.translate("SettingsDialog_base", "&Refresh every:", None, QtGui.QApplication.UnicodeUTF8))
        self.AutoRefreshRateSpinBox.setSuffix(QtGui.QApplication.translate("SettingsDialog_base", " sec", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshTypeGroupBox.setTitle(QtGui.QApplication.translate("SettingsDialog_base", "Devices to Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshCurrentRadioButton.setText(QtGui.QApplication.translate("SettingsDialog_base", "Only &currently selected device", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshAllRadioButton.setText(QtGui.QApplication.translate("SettingsDialog_base", "&All devices", None, QtGui.QApplication.UnicodeUTF8))
        self.TabWidget.setTabText(self.TabWidget.indexOf(self.AutoRefresh), QtGui.QApplication.translate("SettingsDialog_base", "Auto Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel3_2_3.setText(QtGui.QApplication.translate("SettingsDialog_base", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'DejaVu Sans\'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Configure the behavior of the HP Status Service (hp-systray)</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.TabWidget.setTabText(self.TabWidget.indexOf(self.SystemTray), QtGui.QApplication.translate("SettingsDialog_base", "System Tray Icon", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel3_2_2_2.setText(QtGui.QApplication.translate("SettingsDialog_base", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'DejaVu Sans\'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Configure what commands to run for device actions</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_3.setTitle(QtGui.QApplication.translate("SettingsDialog_base", "Commands", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel1_2.setText(QtGui.QApplication.translate("SettingsDialog_base", "&Print:", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel2_2.setText(QtGui.QApplication.translate("SettingsDialog_base", "&Scan:", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel3_3.setText(QtGui.QApplication.translate("SettingsDialog_base", "Send PC &Fax:", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel4.setText(QtGui.QApplication.translate("SettingsDialog_base", "&Unload Photo Cards:", None, QtGui.QApplication.UnicodeUTF8))
        self.textLabel5.setText(QtGui.QApplication.translate("SettingsDialog_base", "Make &Copies:", None, QtGui.QApplication.UnicodeUTF8))
        self.SetDefaultsButton.setText(QtGui.QApplication.translate("SettingsDialog_base", "Set &Defaults", None, QtGui.QApplication.UnicodeUTF8))
        self.TabWidget.setTabText(self.TabWidget.indexOf(self.Commands), QtGui.QApplication.translate("SettingsDialog_base", "Commands (Advanced)", None, QtGui.QApplication.UnicodeUTF8))

from systrayframe import SystrayFrame