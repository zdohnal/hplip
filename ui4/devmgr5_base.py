# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui4/devmgr5_base.ui'
#
# Created: Tue Feb 17 11:36:12 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.setWindowModality(QtCore.Qt.ApplicationModal)
        MainWindow.resize(QtCore.QSize(QtCore.QRect(0,0,700,500).size()).expandedTo(MainWindow.minimumSizeHint()))

        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        self.gridlayout = QtGui.QGridLayout(self.centralwidget)
        self.gridlayout.setObjectName("gridlayout")

        self.splitter = QtGui.QSplitter(self.centralwidget)
        self.splitter.setOrientation(QtCore.Qt.Horizontal)
        self.splitter.setObjectName("splitter")

        self.DeviceList = QtGui.QListWidget(self.splitter)
        self.DeviceList.setMovement(QtGui.QListView.Static)
        self.DeviceList.setFlow(QtGui.QListView.TopToBottom)
        self.DeviceList.setResizeMode(QtGui.QListView.Adjust)
        self.DeviceList.setSpacing(10)
        self.DeviceList.setViewMode(QtGui.QListView.IconMode)
        self.DeviceList.setUniformItemSizes(True)
        self.DeviceList.setWordWrap(True)
        self.DeviceList.setSelectionRectVisible(False)
        self.DeviceList.setObjectName("DeviceList")

        self.Tabs = QtGui.QTabWidget(self.splitter)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.Tabs.sizePolicy().hasHeightForWidth())
        self.Tabs.setSizePolicy(sizePolicy)
        self.Tabs.setObjectName("Tabs")

        self.Actions = QtGui.QWidget()
        self.Actions.setObjectName("Actions")

        self.gridlayout1 = QtGui.QGridLayout(self.Actions)
        self.gridlayout1.setObjectName("gridlayout1")

        self.ActionsList = QtGui.QListWidget(self.Actions)
        self.ActionsList.setSpacing(10)
        self.ActionsList.setViewMode(QtGui.QListView.ListMode)
        self.ActionsList.setUniformItemSizes(True)
        self.ActionsList.setWordWrap(True)
        self.ActionsList.setObjectName("ActionsList")
        self.gridlayout1.addWidget(self.ActionsList,0,0,1,1)
        self.Tabs.addTab(self.Actions,"")

        self.Status = QtGui.QWidget()
        self.Status.setObjectName("Status")

        self.gridlayout2 = QtGui.QGridLayout(self.Status)
        self.gridlayout2.setObjectName("gridlayout2")

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        spacerItem = QtGui.QSpacerItem(21,40,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem)

        self.LCD = QtGui.QLabel(self.Status)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.LCD.sizePolicy().hasHeightForWidth())
        self.LCD.setSizePolicy(sizePolicy)
        self.LCD.setMinimumSize(QtCore.QSize(254,40))
        self.LCD.setMaximumSize(QtCore.QSize(254,40))
        self.LCD.setObjectName("LCD")
        self.hboxlayout.addWidget(self.LCD)

        spacerItem1 = QtGui.QSpacerItem(21,40,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem1)
        self.gridlayout2.addLayout(self.hboxlayout,0,0,1,1)

        self.StatusTable = QtGui.QTableWidget(self.Status)
        self.StatusTable.setAlternatingRowColors(True)
        self.StatusTable.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.StatusTable.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.StatusTable.setShowGrid(False)
        self.StatusTable.setCornerButtonEnabled(False)
        self.StatusTable.setObjectName("StatusTable")
        self.gridlayout2.addWidget(self.StatusTable,1,0,1,1)
        self.Tabs.addTab(self.Status,"")

        self.Supplies = QtGui.QWidget()
        self.Supplies.setObjectName("Supplies")

        self.gridlayout3 = QtGui.QGridLayout(self.Supplies)
        self.gridlayout3.setObjectName("gridlayout3")

        self.SuppliesTable = QtGui.QTableWidget(self.Supplies)
        self.SuppliesTable.setAlternatingRowColors(True)
        self.SuppliesTable.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.SuppliesTable.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.SuppliesTable.setShowGrid(False)
        self.SuppliesTable.setCornerButtonEnabled(False)
        self.SuppliesTable.setObjectName("SuppliesTable")
        self.gridlayout3.addWidget(self.SuppliesTable,0,0,1,1)
        self.Tabs.addTab(self.Supplies,"")

        self.Settings = QtGui.QWidget()
        self.Settings.setObjectName("Settings")

        self.gridlayout4 = QtGui.QGridLayout(self.Settings)
        self.gridlayout4.setObjectName("gridlayout4")

        self.hboxlayout1 = QtGui.QHBoxLayout()
        self.hboxlayout1.setObjectName("hboxlayout1")

        self.PrintSettingsPrinterNameLabel = QtGui.QLabel(self.Settings)
        self.PrintSettingsPrinterNameLabel.setObjectName("PrintSettingsPrinterNameLabel")
        self.hboxlayout1.addWidget(self.PrintSettingsPrinterNameLabel)

        self.PrintSettingsPrinterNameCombo = QtGui.QComboBox(self.Settings)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.PrintSettingsPrinterNameCombo.sizePolicy().hasHeightForWidth())
        self.PrintSettingsPrinterNameCombo.setSizePolicy(sizePolicy)
        self.PrintSettingsPrinterNameCombo.setObjectName("PrintSettingsPrinterNameCombo")
        self.hboxlayout1.addWidget(self.PrintSettingsPrinterNameCombo)
        self.gridlayout4.addLayout(self.hboxlayout1,0,0,1,1)

        self.PrintSettingsToolbox = PrintSettingsToolbox(self.Settings)
        self.PrintSettingsToolbox.setObjectName("PrintSettingsToolbox")
        self.gridlayout4.addWidget(self.PrintSettingsToolbox,1,0,1,1)
        self.Tabs.addTab(self.Settings,"")

        self.Control = QtGui.QWidget()
        self.Control.setObjectName("Control")

        self.gridlayout5 = QtGui.QGridLayout(self.Control)
        self.gridlayout5.setObjectName("gridlayout5")

        self.hboxlayout2 = QtGui.QHBoxLayout()
        self.hboxlayout2.setObjectName("hboxlayout2")

        self.PrintControlPrinterNameLabel = QtGui.QLabel(self.Control)
        self.PrintControlPrinterNameLabel.setObjectName("PrintControlPrinterNameLabel")
        self.hboxlayout2.addWidget(self.PrintControlPrinterNameLabel)

        self.PrintControlPrinterNameCombo = QtGui.QComboBox(self.Control)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.PrintControlPrinterNameCombo.sizePolicy().hasHeightForWidth())
        self.PrintControlPrinterNameCombo.setSizePolicy(sizePolicy)
        self.PrintControlPrinterNameCombo.setObjectName("PrintControlPrinterNameCombo")
        self.hboxlayout2.addWidget(self.PrintControlPrinterNameCombo)
        self.gridlayout5.addLayout(self.hboxlayout2,0,0,1,1)

        self.groupBox = QtGui.QGroupBox(self.Control)
        self.groupBox.setObjectName("groupBox")

        self.gridlayout6 = QtGui.QGridLayout(self.groupBox)
        self.gridlayout6.setObjectName("gridlayout6")

        self.vboxlayout = QtGui.QVBoxLayout()
        self.vboxlayout.setObjectName("vboxlayout")

        self.groupBox_3 = QtGui.QGroupBox(self.groupBox)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred,QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_3.sizePolicy().hasHeightForWidth())
        self.groupBox_3.setSizePolicy(sizePolicy)
        self.groupBox_3.setObjectName("groupBox_3")

        self.gridlayout7 = QtGui.QGridLayout(self.groupBox_3)
        self.gridlayout7.setMargin(1)
        self.gridlayout7.setHorizontalSpacing(6)
        self.gridlayout7.setVerticalSpacing(1)
        self.gridlayout7.setObjectName("gridlayout7")

        self.hboxlayout3 = QtGui.QHBoxLayout()
        self.hboxlayout3.setObjectName("hboxlayout3")

        self.StartStopIcon = QtGui.QLabel(self.groupBox_3)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.StartStopIcon.sizePolicy().hasHeightForWidth())
        self.StartStopIcon.setSizePolicy(sizePolicy)
        self.StartStopIcon.setMinimumSize(QtCore.QSize(16,16))
        self.StartStopIcon.setMaximumSize(QtCore.QSize(16,16))
        self.StartStopIcon.setObjectName("StartStopIcon")
        self.hboxlayout3.addWidget(self.StartStopIcon)

        self.StartStopLabel = QtGui.QLabel(self.groupBox_3)
        self.StartStopLabel.setFrameShape(QtGui.QFrame.NoFrame)
        self.StartStopLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.StartStopLabel.setObjectName("StartStopLabel")
        self.hboxlayout3.addWidget(self.StartStopLabel)
        self.gridlayout7.addLayout(self.hboxlayout3,0,0,1,1)
        self.vboxlayout.addWidget(self.groupBox_3)

        self.StartStopButton = QtGui.QPushButton(self.groupBox)
        self.StartStopButton.setObjectName("StartStopButton")
        self.vboxlayout.addWidget(self.StartStopButton)
        self.gridlayout6.addLayout(self.vboxlayout,0,0,1,1)

        self.vboxlayout1 = QtGui.QVBoxLayout()
        self.vboxlayout1.setObjectName("vboxlayout1")

        self.groupBox_4 = QtGui.QGroupBox(self.groupBox)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred,QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_4.sizePolicy().hasHeightForWidth())
        self.groupBox_4.setSizePolicy(sizePolicy)
        self.groupBox_4.setObjectName("groupBox_4")

        self.gridlayout8 = QtGui.QGridLayout(self.groupBox_4)
        self.gridlayout8.setMargin(1)
        self.gridlayout8.setHorizontalSpacing(6)
        self.gridlayout8.setVerticalSpacing(1)
        self.gridlayout8.setObjectName("gridlayout8")

        self.hboxlayout4 = QtGui.QHBoxLayout()
        self.hboxlayout4.setObjectName("hboxlayout4")

        self.AcceptRejectIcon = QtGui.QLabel(self.groupBox_4)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.AcceptRejectIcon.sizePolicy().hasHeightForWidth())
        self.AcceptRejectIcon.setSizePolicy(sizePolicy)
        self.AcceptRejectIcon.setMinimumSize(QtCore.QSize(16,16))
        self.AcceptRejectIcon.setMaximumSize(QtCore.QSize(16,16))
        self.AcceptRejectIcon.setObjectName("AcceptRejectIcon")
        self.hboxlayout4.addWidget(self.AcceptRejectIcon)

        self.AcceptRejectLabel = QtGui.QLabel(self.groupBox_4)
        self.AcceptRejectLabel.setFrameShape(QtGui.QFrame.NoFrame)
        self.AcceptRejectLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.AcceptRejectLabel.setObjectName("AcceptRejectLabel")
        self.hboxlayout4.addWidget(self.AcceptRejectLabel)
        self.gridlayout8.addLayout(self.hboxlayout4,0,0,1,1)
        self.vboxlayout1.addWidget(self.groupBox_4)

        self.AcceptRejectButton = QtGui.QPushButton(self.groupBox)
        self.AcceptRejectButton.setObjectName("AcceptRejectButton")
        self.vboxlayout1.addWidget(self.AcceptRejectButton)
        self.gridlayout6.addLayout(self.vboxlayout1,0,1,1,1)

        self.vboxlayout2 = QtGui.QVBoxLayout()
        self.vboxlayout2.setObjectName("vboxlayout2")

        self.groupBox_5 = QtGui.QGroupBox(self.groupBox)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred,QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_5.sizePolicy().hasHeightForWidth())
        self.groupBox_5.setSizePolicy(sizePolicy)
        self.groupBox_5.setObjectName("groupBox_5")

        self.gridlayout9 = QtGui.QGridLayout(self.groupBox_5)
        self.gridlayout9.setMargin(1)
        self.gridlayout9.setHorizontalSpacing(6)
        self.gridlayout9.setVerticalSpacing(1)
        self.gridlayout9.setObjectName("gridlayout9")

        self.hboxlayout5 = QtGui.QHBoxLayout()
        self.hboxlayout5.setObjectName("hboxlayout5")

        self.SetDefaultIcon = QtGui.QLabel(self.groupBox_5)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.SetDefaultIcon.sizePolicy().hasHeightForWidth())
        self.SetDefaultIcon.setSizePolicy(sizePolicy)
        self.SetDefaultIcon.setMinimumSize(QtCore.QSize(16,16))
        self.SetDefaultIcon.setMaximumSize(QtCore.QSize(16,16))
        self.SetDefaultIcon.setObjectName("SetDefaultIcon")
        self.hboxlayout5.addWidget(self.SetDefaultIcon)

        self.SetDefaultLabel = QtGui.QLabel(self.groupBox_5)
        self.SetDefaultLabel.setFrameShape(QtGui.QFrame.NoFrame)
        self.SetDefaultLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.SetDefaultLabel.setObjectName("SetDefaultLabel")
        self.hboxlayout5.addWidget(self.SetDefaultLabel)
        self.gridlayout9.addLayout(self.hboxlayout5,0,0,1,1)
        self.vboxlayout2.addWidget(self.groupBox_5)

        self.SetDefaultButton = QtGui.QPushButton(self.groupBox)
        self.SetDefaultButton.setObjectName("SetDefaultButton")
        self.vboxlayout2.addWidget(self.SetDefaultButton)
        self.gridlayout6.addLayout(self.vboxlayout2,0,2,1,1)
        self.gridlayout5.addWidget(self.groupBox,1,0,1,1)

        self.groupBox_2 = QtGui.QGroupBox(self.Control)
        self.groupBox_2.setObjectName("groupBox_2")

        self.gridlayout10 = QtGui.QGridLayout(self.groupBox_2)
        self.gridlayout10.setObjectName("gridlayout10")

        self.JobTable = QtGui.QTableWidget(self.groupBox_2)
        self.JobTable.setAlternatingRowColors(True)
        self.JobTable.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.JobTable.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.JobTable.setGridStyle(QtCore.Qt.DotLine)
        self.JobTable.setObjectName("JobTable")
        self.gridlayout10.addWidget(self.JobTable,0,0,1,3)

        self.CancelJobButton = QtGui.QPushButton(self.groupBox_2)
        self.CancelJobButton.setObjectName("CancelJobButton")
        self.gridlayout10.addWidget(self.CancelJobButton,1,0,1,1)

        spacerItem2 = QtGui.QSpacerItem(131,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout10.addItem(spacerItem2,1,1,1,1)

        self.RefreshButton = QtGui.QPushButton(self.groupBox_2)
        self.RefreshButton.setObjectName("RefreshButton")
        self.gridlayout10.addWidget(self.RefreshButton,1,2,1,1)
        self.gridlayout5.addWidget(self.groupBox_2,2,0,1,1)
        self.Tabs.addTab(self.Control,"")
        self.gridlayout.addWidget(self.splitter,0,0,1,1)
        MainWindow.setCentralWidget(self.centralwidget)

        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0,0,700,27))
        self.menubar.setObjectName("menubar")

        self.menuDevice = QtGui.QMenu(self.menubar)
        self.menuDevice.setObjectName("menuDevice")

        self.menuConfigure = QtGui.QMenu(self.menubar)
        self.menuConfigure.setObjectName("menuConfigure")

        self.menuHelp = QtGui.QMenu(self.menubar)
        self.menuHelp.setObjectName("menuHelp")
        MainWindow.setMenuBar(self.menubar)

        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.toolBar = QtGui.QToolBar(MainWindow)
        self.toolBar.setObjectName("toolBar")
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea,self.toolBar)

        self.QuitAction = QtGui.QAction(MainWindow)
        self.QuitAction.setObjectName("QuitAction")

        self.PreferencesAction = QtGui.QAction(MainWindow)
        self.PreferencesAction.setObjectName("PreferencesAction")

        self.AboutAction = QtGui.QAction(MainWindow)
        self.AboutAction.setObjectName("AboutAction")

        self.ContentsAction = QtGui.QAction(MainWindow)
        self.ContentsAction.setObjectName("ContentsAction")

        self.DeviceSettingsAction = QtGui.QAction(MainWindow)
        self.DeviceSettingsAction.setEnabled(False)
        self.DeviceSettingsAction.setObjectName("DeviceSettingsAction")

        self.DeviceRefreshAction = QtGui.QAction(MainWindow)
        self.DeviceRefreshAction.setObjectName("DeviceRefreshAction")

        self.RefreshAllAction = QtGui.QAction(MainWindow)
        self.RefreshAllAction.setObjectName("RefreshAllAction")

        self.SetupDeviceAction = QtGui.QAction(MainWindow)
        self.SetupDeviceAction.setObjectName("SetupDeviceAction")

        self.RemoveDeviceAction = QtGui.QAction(MainWindow)
        self.RemoveDeviceAction.setObjectName("RemoveDeviceAction")
        self.menuDevice.addAction(self.DeviceSettingsAction)
        self.menuDevice.addSeparator()
        self.menuDevice.addAction(self.DeviceRefreshAction)
        self.menuDevice.addAction(self.RefreshAllAction)
        self.menuDevice.addSeparator()
        self.menuDevice.addAction(self.SetupDeviceAction)
        self.menuDevice.addAction(self.RemoveDeviceAction)
        self.menuDevice.addSeparator()
        self.menuDevice.addAction(self.QuitAction)
        self.menuConfigure.addAction(self.PreferencesAction)
        self.menuHelp.addAction(self.ContentsAction)
        self.menuHelp.addAction(self.AboutAction)
        self.menubar.addAction(self.menuDevice.menuAction())
        self.menubar.addAction(self.menuConfigure.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())
        self.toolBar.addAction(self.DeviceRefreshAction)
        self.toolBar.addAction(self.RefreshAllAction)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.SetupDeviceAction)
        self.toolBar.addAction(self.RemoveDeviceAction)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.ContentsAction)

        self.retranslateUi(MainWindow)
        self.Tabs.setCurrentIndex(0)
        self.PrintSettingsToolbox.setCurrentIndex(-1)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "HP Device Manager", None, QtGui.QApplication.UnicodeUTF8))
        self.Tabs.setTabText(self.Tabs.indexOf(self.Actions), QtGui.QApplication.translate("MainWindow", "Actions", None, QtGui.QApplication.UnicodeUTF8))
        self.StatusTable.clear()
        self.StatusTable.setColumnCount(0)
        self.StatusTable.setRowCount(0)
        self.Tabs.setTabText(self.Tabs.indexOf(self.Status), QtGui.QApplication.translate("MainWindow", "Status", None, QtGui.QApplication.UnicodeUTF8))
        self.SuppliesTable.clear()
        self.SuppliesTable.setColumnCount(0)
        self.SuppliesTable.setRowCount(0)
        self.Tabs.setTabText(self.Tabs.indexOf(self.Supplies), QtGui.QApplication.translate("MainWindow", "Supplies", None, QtGui.QApplication.UnicodeUTF8))
        self.PrintSettingsPrinterNameLabel.setText(QtGui.QApplication.translate("MainWindow", "Printer Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.Tabs.setTabText(self.Tabs.indexOf(self.Settings), QtGui.QApplication.translate("MainWindow", "Print Settings", None, QtGui.QApplication.UnicodeUTF8))
        self.PrintControlPrinterNameLabel.setText(QtGui.QApplication.translate("MainWindow", "Printer Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("MainWindow", "Printer/Queue Control", None, QtGui.QApplication.UnicodeUTF8))
        self.StartStopLabel.setText(QtGui.QApplication.translate("MainWindow", "Started", None, QtGui.QApplication.UnicodeUTF8))
        self.StartStopButton.setText(QtGui.QApplication.translate("MainWindow", "Stop Printer", None, QtGui.QApplication.UnicodeUTF8))
        self.AcceptRejectLabel.setText(QtGui.QApplication.translate("MainWindow", "Accepting", None, QtGui.QApplication.UnicodeUTF8))
        self.AcceptRejectButton.setText(QtGui.QApplication.translate("MainWindow", "Reject Jobs", None, QtGui.QApplication.UnicodeUTF8))
        self.SetDefaultLabel.setText(QtGui.QApplication.translate("MainWindow", "Not default", None, QtGui.QApplication.UnicodeUTF8))
        self.SetDefaultButton.setText(QtGui.QApplication.translate("MainWindow", "Set as Default", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_2.setTitle(QtGui.QApplication.translate("MainWindow", "Job Queue", None, QtGui.QApplication.UnicodeUTF8))
        self.JobTable.clear()
        self.JobTable.setColumnCount(0)
        self.JobTable.setRowCount(0)
        self.CancelJobButton.setText(QtGui.QApplication.translate("MainWindow", "Cancel Job", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshButton.setText(QtGui.QApplication.translate("MainWindow", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.Tabs.setTabText(self.Tabs.indexOf(self.Control), QtGui.QApplication.translate("MainWindow", "Printer Control", None, QtGui.QApplication.UnicodeUTF8))
        self.menuDevice.setTitle(QtGui.QApplication.translate("MainWindow", "&Device", None, QtGui.QApplication.UnicodeUTF8))
        self.menuConfigure.setTitle(QtGui.QApplication.translate("MainWindow", "&Configure", None, QtGui.QApplication.UnicodeUTF8))
        self.menuHelp.setTitle(QtGui.QApplication.translate("MainWindow", "&Help", None, QtGui.QApplication.UnicodeUTF8))
        self.toolBar.setWindowTitle(QtGui.QApplication.translate("MainWindow", "toolBar_2", None, QtGui.QApplication.UnicodeUTF8))
        self.QuitAction.setText(QtGui.QApplication.translate("MainWindow", "Quit", None, QtGui.QApplication.UnicodeUTF8))
        self.PreferencesAction.setText(QtGui.QApplication.translate("MainWindow", "Preferences...", None, QtGui.QApplication.UnicodeUTF8))
        self.AboutAction.setText(QtGui.QApplication.translate("MainWindow", "About...", None, QtGui.QApplication.UnicodeUTF8))
        self.ContentsAction.setText(QtGui.QApplication.translate("MainWindow", "Contents...", None, QtGui.QApplication.UnicodeUTF8))
        self.DeviceSettingsAction.setText(QtGui.QApplication.translate("MainWindow", "Settings...", None, QtGui.QApplication.UnicodeUTF8))
        self.DeviceRefreshAction.setText(QtGui.QApplication.translate("MainWindow", "Refresh Device", None, QtGui.QApplication.UnicodeUTF8))
        self.RefreshAllAction.setText(QtGui.QApplication.translate("MainWindow", "Refresh All", None, QtGui.QApplication.UnicodeUTF8))
        self.SetupDeviceAction.setText(QtGui.QApplication.translate("MainWindow", "Setup Device...", None, QtGui.QApplication.UnicodeUTF8))
        self.RemoveDeviceAction.setText(QtGui.QApplication.translate("MainWindow", "Remove Device...", None, QtGui.QApplication.UnicodeUTF8))

from printsettingstoolbox import PrintSettingsToolbox