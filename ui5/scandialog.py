#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Hp-Scan.ui'
#
# Created by: PyQt5 UI code generator 4.11.4
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets
from base import utils,imageprocessing
#from scan import sane
import re
import os
import platform

from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from scan import sane

#devicelist = {}
device_name = ''
path = os.getcwd()
new_path = os.getcwd()
multipick_error_message = "The scan operation has been cancelled or a multipick or paper is jammed in the ADF.\nIf you cancelled the scan,click OK.\nIf the scan was terminated due to a multi-feed or paper jam in the ADF,\ndo the following:\n\n1)Clear the ADF path. For instructions see your product documentation.\n2)Check the sheets are not stuck together. Remove any staples, sticky notes,tape or other objects.\n3)Restart the scan\n\nNote:If necessary, turn off automatic detection of multi-pick before starting a new scan\n"
convert_error_message = "Convert command not found. Multiple Tiff document generation,\n Batch seperation feature with Tiff file format,\n Page merge feature and PDF generation using reportlab may not work as excepted.\n Please install ImageMagick package and try again\n"


no_document_error_message = "No document(s). Please load documents and try again."

no_pages_to_merge = "No scanned documents to merge."
pyPlatform = ''
num= {}
try:
    _encoding = QtWidgets.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtCore.QCoreApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtCore.QCoreApplication.translate(context, text, disambig)

class Ui_HpScan(object):
    devicelist = {}
    #device_name = ''
    file_type = 'png'
    source = ''
    color = 'gray'
    resolution = '300'
    size ='letter'
    device_uri = ''
    multi_pick = False
    document_merge =False
    auto_orient =False
    crushed = False
    #searchablePDF = False
    mixed_feed =False
    blank_page = False
    batch_seperation = False
    bp_barcode = False
    auto_crop = False
    deskew_image = False
    document_merge_adf_flatbed = False
    image_enhancement = False
    brightness = False
    contrast = False
    sharpness = False
    color_value = False
    sizel1 = 0
    sizel2 = 0
    sizel3 = 0
    sizel4 = 0
    deskew_image_pri = True
    auto_crop_pri = True
    mixed_feed_pri = True
    auto_orient_pri = True
    document_merge_adf_flatbed_pri = True
    multi_pick_pri = True
    #batch_seperation_pri = True
    crushed_pri = True
    document_merge_pri = True
    image_enhancement_pri = True
    blank_page_pri = True
    batchsepBC_pri = True
    other_device_cnt = 0
	
    def setupUi(self, HpScan):
        pyPlatform = platform.python_version()
        HpScan.setObjectName("HpScan")
        HpScan.setMinimumSize(QtCore.QSize(680, 620))
        HpScan.setMaximumSize(QtCore.QSize(680, 620))
        font = QtGui.QFont()
        font.setBold(False)
        font.setWeight(50)
        HpScan.setFont(font)
        HpScan.setMouseTracking(False)
        HpScan.setFocusPolicy(QtCore.Qt.NoFocus)
        self.dockWidgetContents = QtWidgets.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.label_Type = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Type.setGeometry(QtCore.QRect(10, 89, 51, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label_Type.setFont(font)
        self.label_Type.setMouseTracking(True)
        self.label_Type.setObjectName("label_Type")
        self.comboBox_Type = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Type.setGeometry(QtCore.QRect(85, 90, 171, 41))
        self.comboBox_Type.setObjectName("comboBox_Type")
        self.comboBox_Type.addItem("")
        self.comboBox_Type.addItem("")
        self.comboBox_Type.addItem("")
        self.comboBox_Type.addItem("")
        self.comboBox_Type.currentIndexChanged.connect(self.comboBox_TypeIndexChanged)
        self.comboBox_Flatbed = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Flatbed.setGeometry(QtCore.QRect(85, 150, 171, 41))
        self.comboBox_Flatbed.setObjectName("comboBox_Flatbed")
        #self.comboBox_Flatbed.addItem(_fromUtf8(""))
        #self.comboBox_Flatbed.addItem(_fromUtf8(""))
        #self.comboBox_Flatbed.addItem(_fromUtf8(""))
        #self.comboBox_Flatbed.currentIndexChanged.connect(self.comboBox_SourceChanged)
        self.comboBox_Color = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Color.setGeometry(QtCore.QRect(85, 210, 171, 41))
        self.comboBox_Color.setObjectName("comboBox_Color")
        self.comboBox_Color.addItem("")
        self.comboBox_Color.addItem("")
        self.comboBox_Color.currentIndexChanged.connect(self.comboBox_ColorIndexChanged)
        self.comboBox_Resolution = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Resolution.setGeometry(QtCore.QRect(85, 270, 171, 41))
        self.comboBox_Resolution.setObjectName("comboBox_Resolution")
        self.comboBox_Resolution.addItem("")
        self.comboBox_Resolution.addItem("")
        self.comboBox_Resolution.addItem("")
        self.comboBox_Resolution.addItem("")
        self.comboBox_Resolution.addItem("")
        self.comboBox_Resolution.currentIndexChanged.connect(self.comboBox_ResIndexChanged)
        self.label_Size = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Size.setGeometry(QtCore.QRect(10, 329, 51, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label_Size.setFont(font)
        self.label_Size.setMouseTracking(True)
        self.label_Size.setObjectName("label_Size")
        self.label_Device = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Device.setGeometry(QtCore.QRect(10, 29, 65, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label_Device.setFont(font)
        self.label_Device.setMouseTracking(True)
        self.label_Device.setObjectName("label_Device")
        self.comboBox_Papersize = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Papersize.setGeometry(QtCore.QRect(85, 330, 171, 41))
        self.comboBox_Papersize.setObjectName("comboBox_Papersize")
        self.comboBox_Papersize.addItem("")
        self.comboBox_Papersize.addItem("")
        self.comboBox_Papersize.addItem("")
        self.comboBox_Papersize.addItem("")
        self.comboBox_Papersize.addItem("")
        self.comboBox_Papersize.currentIndexChanged.connect(self.comboBox_PaperSizeIndexChanged)
        self.pushButton_Scan = QtWidgets.QPushButton(self.dockWidgetContents)
        self.pushButton_Scan.setGeometry(QtCore.QRect(60, 470, 81, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.pushButton_Scan.setFont(font)        
        self.pushButton_Scan.setObjectName("pushButton_Scan")
        self.pushButton_Scan.clicked.connect(self.scanButton_clicked)

        self.label_Path = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Path.setGeometry(QtCore.QRect(10,390, 51, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label_Path.setFont(font)
        self.label_Path.setMouseTracking(True)
        self.label_Path.setObjectName("label_Path")
        
        self.pushButton_Change = QtWidgets.QPushButton(self.dockWidgetContents)
        self.pushButton_Change.setGeometry(QtCore.QRect(155, 470, 101, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.pushButton_Change.setFont(font)
        self.pushButton_Change.setObjectName("pushButton_Change")
        self.pushButton_Change.clicked.connect(self.selectFile)
        
        self.pushButton_Merge = QtWidgets.QPushButton(self.dockWidgetContents)
        self.pushButton_Merge.setGeometry(QtCore.QRect(450, 355, 81, 31))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.pushButton_Merge.setFont(font)
        self.pushButton_Merge.setObjectName("pushButton_Merge")
        self.pushButton_Merge.setEnabled(False)
        self.pushButton_Merge.clicked.connect(self.mergeButton_clicked)

        self.label_Flatbed = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Flatbed.setGeometry(QtCore.QRect(10, 150, 51, 41))
        self.label_Flatbed.setText("")
        self.label_Flatbed.setPixmap(QtGui.QPixmap("/usr/share/hplip/data/images/other/flat1.png"))
        self.label_Flatbed.setObjectName("label_Flatbed")
        self.label_Color = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Color.setGeometry(QtCore.QRect(20, 210, 61, 41))
        self.label_Color.setText("")
        self.label_Color.setPixmap(QtGui.QPixmap("/usr/share/hplip/data/images/other/viewer.png"))
        self.label_Color.setObjectName("label_Color")
        self.label_Resolution = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Resolution.setGeometry(QtCore.QRect(20, 270, 51, 51))
        self.label_Resolution.setText("")
        self.label_Resolution.setPixmap(QtGui.QPixmap("/usr/share/hplip/data/images/other/resolution.png"))
        self.label_Resolution.setObjectName("label_Resolution")
        self.auto_orient = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.auto_orient.setGeometry(QtCore.QRect(300,60,117, 22))
        self.auto_orient.setObjectName("auto_orient")
        self.auto_orient.stateChanged.connect(self.Auto_orient)
        self.crushed = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.crushed.setGeometry(QtCore.QRect(300,390,200, 22))
        self.crushed.setObjectName("crushed")
        self.crushed.stateChanged.connect(self.Crushed)
        #self.searchablePDF = QtGui.QCheckBox(self.dockWidgetContents)
        #self.searchablePDF.setGeometry(QtCore.QRect(300,420,200, 22))
        #self.searchablePDF.setObjectName(_fromUtf8("searchablePDF"))
        #self.searchablePDF.stateChanged.connect(self.SearchablePDF)
        self.auto_crop = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.auto_crop.setGeometry(QtCore.QRect(300, 90, 241, 20))
        self.auto_crop.setObjectName("auto_crop")
        self.auto_crop.stateChanged.connect(self.Auto_crop)
        self.multi_pick = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.multi_pick.setGeometry(QtCore.QRect(300, 120, 231, 22))
        self.multi_pick.setObjectName("multi_pick")
        self.multi_pick.stateChanged.connect(self.Multi_pick)
        self.blank_page = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.blank_page.setGeometry(QtCore.QRect(300, 150, 241, 22))
        self.blank_page.setObjectName("blank_page")
        self.blank_page.stateChanged.connect(self.Blank_page)
        self.batch_seperation = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.batch_seperation.setGeometry(QtCore.QRect(300, 180, 201, 22))
        self.batch_seperation.setObjectName("batch_seperation")
        self.batch_seperation.stateChanged.connect(self.batch_Seperation)
        #self.batch_seperation.setEnabled(False)
        self.bp_blankpage = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.bp_blankpage.setGeometry(QtCore.QRect(330, 210, 201, 22))
        self.bp_blankpage.setObjectName("bp_blankpage")
        self.bp_blankpage.setEnabled(False)
        self.bp_barcode = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.bp_barcode.setGeometry(QtCore.QRect(330, 240, 311, 22))
        self.bp_barcode.setObjectName("bp_barcode")
        self.bp_barcode.setEnabled(False)
        self.comboBox_Barcode_Type = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Barcode_Type.setGeometry(QtCore.QRect(360, 270, 261, 27))
        self.comboBox_Barcode_Type.setObjectName("comboBox_Barcode_Type")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.addItem("")
        self.comboBox_Barcode_Type.setEnabled(False)
        #self.comboBox_Barcode_Type.currentIndexChanged.connect(self.comboBox_ResIndexChanged)
        self.document_merge = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.document_merge.setGeometry(QtCore.QRect(300, 300, 161, 22))
        self.document_merge.setObjectName("document_merge")
        self.document_merge.stateChanged.connect(self.Document_merge)
        self.mixed_feed = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.mixed_feed.setGeometry(QtCore.QRect(300, 330, 200, 22))
        self.mixed_feed.setObjectName("mixed_feed")
        self.mixed_feed.stateChanged.connect(self.Mixed_feed)
        self.deskew_image = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.document_merge_adf_flatbed = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.document_merge_adf_flatbed.setGeometry(QtCore.QRect(300, 360, 150, 22))
        self.document_merge_adf_flatbed.setObjectName("document_merge_adf_flatbed")
        self.document_merge_adf_flatbed.stateChanged.connect(self.Document_merge_adf_flatbed)
        self.label_Brightness = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Brightness.setGeometry(QtCore.QRect(310,445, 250, 22))
        #font = QtGui.QFont()
        #font.setBold(True)
        #font.setWeight(75)
        #self.label_Brightness.setFont(font)
        self.label_Brightness.setMouseTracking(True)
        self.label_Brightness.setObjectName("label_Brightness")
        self.label_Contrast = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Contrast.setGeometry(QtCore.QRect(310,475, 250, 22))
        #font = QtGui.QFont()
        #font.setBold(True)
        #font.setWeight(75)
        #self.label_Contrast.setFont(font)
        self.label_Contrast.setMouseTracking(True)
        self.label_Contrast.setObjectName("label_Contrast")
        self.label_Sharpness = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Sharpness.setGeometry(QtCore.QRect(310,505, 250, 22))
        #font = QtGui.QFont()
        #font.setBold(True)
        #font.setWeight(75)
        #self.label_Sharpness.setFont(font)
        self.label_Sharpness.setMouseTracking(True)
        self.label_Sharpness.setObjectName("label_Sharpness")
        self.label_Color_value = QtWidgets.QLabel(self.dockWidgetContents)
        self.label_Color_value.setGeometry(QtCore.QRect(310,535, 250, 22))
        #font = QtGui.QFont()
        #font.setBold(True)
        #font.setWeight(75)
        #self.label_Color_value.setFont(font)
        self.label_Color_value.setMouseTracking(True)
        self.label_Color_value.setObjectName("label_Color_value")
        self.image_enhancement = QtWidgets.QCheckBox(self.dockWidgetContents)
        self.image_enhancement.setGeometry(QtCore.QRect(300, 420, 250, 22))
        self.image_enhancement.setObjectName("image_enhancement")
        self.image_enhancement.stateChanged.connect(self.Image_enhancement)
        self.deskew_image.setGeometry(QtCore.QRect(300,30, 241, 20))
        self.deskew_image.setObjectName("deskew_image")
        self.deskew_image.stateChanged.connect(self.Deskew_image)
        self.comboBox_Device_URI = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_Device_URI.setGeometry(QtCore.QRect(85, 30, 171, 41))
        self.comboBox_Device_URI.setObjectName("comboBox_Device_URI")
        self.comboBox_Device_URI.currentIndexChanged.connect(self.comboBox_device_URI)
        
        self.comboBox_path = QtWidgets.QComboBox(self.dockWidgetContents)
        self.comboBox_path.setEditable(False)
        self.comboBox_path.setGeometry(QtCore.QRect(85,390, 171, 41))
        self.comboBox_path.setObjectName("comboBox_path")
        self.comboBox_path.currentIndexChanged.connect(self.comboBox_Path)
        
        #self.layout = QtGui.QVBoxLayout(self.dockWidgetContents)

        
        #self.s1 = QtGui.QSlider(Qt.Horizontal)
        self.s1 = QtWidgets.QSlider(Qt.Horizontal,self.dockWidgetContents)
        self.s1.setFocusPolicy(QtCore.Qt.NoFocus)
        self.s1.setGeometry(410, 440, 100, 30)

        #self.s1 = QSlider(Qt.Horizontal)
        #self.s1.setGeometry(320, 460, 250, 22)
        self.s1.setMinimum(0)
        self.s1.setMaximum(255)
        self.s1.setValue(100)

        self.s1.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.s1.setTickInterval(10)
        #self.s1.setSliderPosition(5)
        #self.layout.addWidget(self.s1,0,QtCore.Qt.AlignRight)
        #HpScan.addWidget(self.s1,0,QtCore.Qt.AlignRight)
        self.s1.setEnabled(False)
        self.s1.valueChanged.connect(self.valuechange_brightness)
        
        self.s2 = QtWidgets.QSlider(Qt.Horizontal,self.dockWidgetContents)
        self.s2.setFocusPolicy(QtCore.Qt.NoFocus)
        self.s2.setGeometry(410, 470, 100, 30)
        #self.s2 = QSlider(Qt.Horizontal)
        #self.s2.setGeometry(1320, 480, 250, 22)
        self.s2.setMinimum(0)
        self.s2.setMaximum(255)
        self.s2.setValue(100)
        self.s2.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.s2.setTickInterval(10)

        #self.layout.addWidget(self.s2,0,QtCore.Qt.AlignRight)
        self.s2.setEnabled(False)
        
        #self.layout.setObjectName(_fromUtf8("brightness"))
        self.s2.valueChanged.connect(self.valuechange_contrast)
        #self.setLayout(layout)
        
        self.s3 = QtWidgets.QSlider(Qt.Horizontal,self.dockWidgetContents)
        self.s3.setFocusPolicy(QtCore.Qt.NoFocus)
        self.s3.setGeometry(410, 500, 100, 30)
        #self.s3 = QSlider(Qt.Horizontal)
        #self.s3.setGeometry(1320, 500, 250, 22)
        self.s3.setMinimum(0)
        self.s3.setMaximum(200)
        self.s3.setValue(100)
        self.s3.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.s3.setTickInterval(10)

        #self.layout.addWidget(self.s3,0,QtCore.Qt.AlignRight)
        self.s3.setEnabled(False)
        self.s3.valueChanged.connect(self.valuechange_sharpness)
        
        self.s4 = QtWidgets.QSlider(Qt.Horizontal,self.dockWidgetContents)
        self.s4.setFocusPolicy(QtCore.Qt.NoFocus)
        self.s4.setGeometry(410, 530, 100, 30)
        #self.s4 = QSlider(Qt.Horizontal)
        #self.s4.setGeometry(1320, 520, 250, 22)
        self.s4.setMinimum(0)
        self.s4.setMaximum(255)
        self.s4.setValue(100)
        self.s4.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.s4.setTickInterval(10)

        #self.layout.addWidget(self.s4,0,QtCore.Qt.AlignRight)
        self.s4.setEnabled(False)
        self.s4.valueChanged.connect(self.valuechange_color)
        
        
        #self.pushButton_Cancel = QtGui.QPushButton(self.dockWidgetContents)
        #self.pushButton_Cancel.setGeometry(QtCore.QRect(150, 270, 99, 41))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        #self.pushButton_Cancel.setFont(font)
        #self.pushButton_Cancel.setObjectName(_fromUtf8("pushButton_Cancel"))
        HpScan.setWidget(self.dockWidgetContents)

        self.retranslateUi(HpScan)
        QtCore.QMetaObject.connectSlotsByName(HpScan)
        #self.initUI()
    def valuechange_brightness(self):
        #print "entered value changed brightness" 
        #global sizel1
        self.brightness = True
        self.sizel1 = self.s1.value()
        #print "printing new value brightness"
        #print self.sizel1
    def valuechange_contrast(self):
        #print "entered value changed contrast" 
        #global sizel2
        self.contrast = True
        self.sizel2 = self.s2.value()
        #print "printing new value contrast"
        #print self.sizel2
    def valuechange_sharpness(self):
        #print "entered value changed sharpness" 
        #global sizel3
        self.sharpness = True
        self.sizel3 = self.s3.value()
        #print "printing new value sharpness"
        #print self.sizel3
    def valuechange_color(self):
        #print "entered value changed color" 
        #global sizel4
        self.color_value = True
        self.sizel4 = self.s4.value()
        #print "printing new value color"
        #print self.sizel4
    #def initUI(self):
        #self.connect(self.pushButton_Scan,SIGNAL("clicked()"),self.scanButton_clicked())
    def comboBox_Path(self, new_path = None):
        path = new_path
        
    def selectFile(self):
        global path
        new_path = QFileDialog.getExistingDirectory(None)
        if new_path:
            #print "entered"
            #print new_path
            self.comboBox_Path(new_path)
            self.comboBox_path.setItemText(0,_translate("HpScan", new_path, None))
            path = new_path
    
    def scanButton_clicked(self):
        cmd = "hp-scan" + ' --device=' + self.device_uri + ' --filetype=' + self.file_type + ' --mode=' + self.color + ' --res=' + self.resolution + ' --size=' + self.size
        if self.source == 'adf' or self.source == 'duplex':
            cmd = cmd + ' --' + self.source
        if self.multi_pick.isChecked() == True:
            cmd = cmd + ' --' + 'multipick'
        if self.auto_orient.isChecked() == True:
            cmd = cmd + ' --' + 'autoorient'
        if self.crushed.isChecked() == True:
            cmd = cmd + ' --' + 'crushed'
        #if self.searchablePDF.isChecked() == True:
	    #cmd = cmd + ' --' + 'searchablePDF'
        if self.mixed_feed.isChecked() == True:
            cmd = re.sub(r'\--size=.+\ ', '', cmd)
            cmd = cmd + ' --' + 'mixedfeed'
        if self.document_merge.isChecked() == True:
            cmd = cmd + ' --' + 'docmerge'
        if self.auto_crop.isChecked() == True:
            cmd = cmd + ' --' + 'autocrop'
        if self.deskew_image.isChecked() == True:
            cmd = cmd + ' --' + 'deskew'
        if self.blank_page.isChecked() == True:
            cmd = cmd + ' --' + 'blankpage'
        if self.document_merge_adf_flatbed.isChecked() == True:
            cmd = cmd + ' --' + 'adf_fladbed_merge'
        if self.image_enhancement.isChecked() == True and self.brightness == True:
            cmd = cmd + ' --' + 'brightness' + '=' + str(self.sizel1)
        if self.image_enhancement.isChecked() == True and self.contrast == True:
            cmd = cmd + ' --' + 'contrast' + '=' + str(self.sizel2)
        if self.image_enhancement.isChecked() == True and self.sharpness == True:
            cmd = cmd + ' --' + 'sharpness' + '=' + str(self.sizel3)
        if self.image_enhancement.isChecked() == True and self.color_value == True:
            cmd = cmd + ' --' + 'color_value' + '=' + str(self.sizel4)
        if self.batch_seperation.isChecked() == True:
            if self.bp_blankpage.isChecked() == True:
                cmd = cmd + ' --' + 'batchsepBP'
            elif self.bp_barcode.isChecked() == True:
                cmd = cmd + ' --' + 'batchsepBC'
            else:
                self.failureMessage("Select either barcode or blankpage option for separation")
        #if self.bp_barcode.isChecked() == True:
            #cmd = cmd + ' --' + 'batchsepBC'
        cmd = cmd + ' --path=' + str(path)
        cmd = cmd + ' --' + 'uiscan'
        #print (cmd)
        self.pushButton_Scan.setEnabled(False)
        status = utils.run(cmd)
        #print (status)
        if status[0] == 2:
            self.failureMessage(multipick_error_message)
        elif status[0] == 3:
            self.warningMessage(no_document_error_message)
        elif status[0] == 4:
            output_pdf = status[1].split("error: ", 1)[1]
            output_pdf = output_pdf.split('.pdf', 1)[0]+".pdf"
            imageprocessing.merge_PDF_viewer(output_pdf)
        elif status[0] == 5:
            output_pdf = status[1].split("error: ", 1)[1]
            #print output_pdf
            output_pdf = output_pdf.split(']', 1)[0]+"]"
            #print output_pdf 
            for char in output_pdf:
                #print char
                if char in "[']":
                    output_pdf = output_pdf.replace(char,'')
            output_pdf = output_pdf.split(',')
            #print output_pdf
            #print type(output_pdf)
            for p in output_pdf:
                imageprocessing.merge_PDF_viewer(p)
        elif status[0] == 6:
            self.failureMessage(convert_error_message)
        self.pushButton_Scan.setEnabled(True)
        #if status != 0:
            #print("Cmd %s failed with status %d",cmd,status)
        #sys.exit(app.exec_())


    def msgbtn(self):
        pass

    def failureMessage(self,message):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Information)
        msg.setText(message)
        msg.setStandardButtons(QMessageBox.Ok)
        msg.buttonClicked.connect(self.msgbtn)
        retval = msg.exec_()
    def warningMessage(self,message):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Information)
        msg.setText(message)
        msg.setStandardButtons(QMessageBox.Ok)
        msg.buttonClicked.connect(self.msgbtn)
        retval = msg.exec_()

    def comboBox_TypeIndexChanged(self):
        self.file_type = str(self.comboBox_Type.currentText()).lower()
        #print self.file_type

    def comboBox_SourceChanged(self,device):
        if device != '5000' and device != '7500':
            self.multi_pick_pri = False
        else:
            self.comboBox_Flatbed.clear()
            self.comboBox_Flatbed.addItem("")
            self.comboBox_Flatbed.addItem("")
            if device == '5000': 
                self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "Duplex", None))
                self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "ADF", None))
                self.comboBox_Flatbed.setCurrentIndex(1)
            elif device == '7500':
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "Flatbed", None))
                self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "ADF", None))
                self.comboBox_Flatbed.setItemText(2, _translate("HpScan", "Duplex", None))
                self.comboBox_Flatbed.setCurrentIndex(1)
            if device == '5000' or device == '7500':
                self.source = str(self.comboBox_Flatbed.currentText()).lower()
                self.comboBox_Flatbed.currentIndexChanged.connect(self.comboBox_SourceSelected)
        
    def comboBox_SourceSelected(self):
        self.source = str(self.comboBox_Flatbed.currentText()).lower()
        if self.source == 'flatbed':
            self.source = ''
        #print self.source

    def comboBox_ColorIndexChanged(self):
        self.color = str(self.comboBox_Color.currentText()).lower()
        #print self.color

    def comboBox_ResIndexChanged(self):
        self.resolution = str(self.comboBox_Resolution.currentText()).lower()
        #print self.resolution

    def comboBox_PaperSizeIndexChanged(self):
        self.size = str(self.comboBox_Papersize.currentText())
        #print self.size

    def batch_Seperation(self):
        if self.batch_seperation.isChecked() == True:
            pyPlatform = platform.python_version()
            num = pyPlatform.split('.')
            if num[0] >= '3':
                self.bp_barcode.setChecked(False)
                self.bp_barcode.setEnabled(False)
                self.CheckEnable()
                self.bp_blankpage.setEnabled(True)
                self.bp_blankpage.stateChanged.connect(self.bp_Blankpage)
            else:
                self.CheckEnable()	    
                self.bp_blankpage.setEnabled(True)
                if self.batchsepBC_pri == True: 
                    self.bp_barcode.setEnabled(True)
                #self.blank_page.setChecked(False)    
                #self.blank_page.setText(_translate("HpScan", "Blank page seperation removal ", None))
                #if pyPlatform < '3':
                self.bp_barcode.stateChanged.connect(self.bp_Barcode)
                self.bp_blankpage.stateChanged.connect(self.bp_Blankpage)
        else:
            #if pyPlatform < 3:
            self.CheckEnable()
            self.bp_blankpage.setChecked(False)
            self.bp_barcode.setChecked(False)
            self.bp_blankpage.setEnabled(False)
            self.bp_barcode.setEnabled(False)
            #self.blank_page.setChecked(False)
            #self.blank_page.setEnabled(True)    
            #self.blank_page.setText(_translate("HpScan", "Delete blank page ", None))

    def bp_Barcode(self):
        pyPlaform = platform.python_version()
        num = pyPlatform.split('.')
        if num[0] >= '3':
            self.bp_barcode.setEnabled(False)
            self.comboBox_Barcode_Type.setEnabled(False)
        else:
            if self.bp_barcode.isChecked() == True:	    
                self.comboBox_Barcode_Type.setEnabled(True)
                self.bp_blankpage.setEnabled(False)
            else:
                self.comboBox_Barcode_Type.setEnabled(False)
                self.bp_blankpage.setEnabled(True)

    def bp_Blankpage(self):
        if self.bp_blankpage.isChecked() == True:
            if self.blank_page_pri == True:
                self.blank_page.setEnabled(True)
            self.bp_barcode.setEnabled(False)
            self.blank_page.setChecked(False)    
            self.blank_page.setText(_translate("HpScan", "Blank page seperation removal ", None))
        else:
            if self.blank_page_pri == True:
                self.blank_page.setEnabled(True) 
            pyPlatform = platform.python_version()
            num = pyPlatform.split('.')
            if num[0] >= '3':
                self.bp_barcode.setEnabled(False)
            else:
                if self.batchsepBC_pri == True:
                    self.bp_barcode.setEnabled(True)
            self.blank_page.setChecked(False)
            #self.blank_page.setEnabled(True)    
            self.blank_page.setText(_translate("HpScan", "Delete blank page ", None))

    def Multi_pick(self):
        if self.multi_pick.isChecked() == True:
            self.comboBox_Flatbed.clear()
            self.comboBox_Flatbed.addItem("")
            self.comboBox_Flatbed.addItem("")
            self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "Duplex", None))
            self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "ADF", None))
            self.comboBox_Flatbed.setCurrentIndex(1)
            if self.multi_pick_pri == True:	    
                self.multi_pick.setEnabled(True)
            self.CheckEnable()
        else:
            if (re.search(r'_7500', self.device_uri)):
                self.comboBox_Flatbed.clear()
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "Flatbed", None))
                self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "ADF", None))
                self.comboBox_Flatbed.setItemText(2, _translate("HpScan", "Duplex", None))
                self.comboBox_Flatbed.setCurrentIndex(1)
            elif (re.search(r'_5000_', self.device_uri)):
                self.comboBox_Flatbed.clear()
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.addItem("")
                self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "ADF", None))
                self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "Duplex", None))
                self.comboBox_Flatbed.setCurrentIndex(0)
            self.CheckEnable()
    
    def Auto_orient(self):
        if self.auto_orient.isChecked() == True:
            if self.auto_orient_pri == True:	    
                self.auto_orient.setEnabled(True)
            self.CheckEnable()
        else:
            self.CheckEnable()

    def CheckEnable(self):
        if self.auto_orient.isChecked() == False and self.auto_crop.isChecked() == False and self.image_enhancement.isChecked() == False and self.deskew_image.isChecked() == False and self.blank_page.isChecked() == False and self.document_merge_adf_flatbed.isChecked() == False and self.multi_pick.isChecked() == False and self.batch_seperation.isChecked() == False:
            self.crushed.setChecked(False)
            self.mixed_feed.setChecked(False)
            self.document_merge.setChecked(False)
            if self.crushed_pri == True:
                self.crushed.setEnabled(True)
            if self.mixed_feed_pri == True:
                self.mixed_feed.setEnabled(True)
            if self.document_merge_pri == True:
                self.document_merge.setEnabled(True)
        else:
            self.crushed.setChecked(False)
            self.mixed_feed.setChecked(False)
            self.document_merge.setChecked(False)
            self.crushed.setEnabled(False)
            self.mixed_feed.setEnabled(False)
            self.document_merge.setEnabled(False)

    def DisableAllScanjet(self):
        self.auto_orient.setChecked(False)
        self.auto_crop.setChecked(False)
        self.image_enhancement.setChecked(False)
        self.document_merge_adf_flatbed.setChecked(False)
        self.multi_pick.setChecked(False)
        self.batch_seperation.setChecked(False)
        self.deskew_image.setChecked(False)
        self.blank_page.setChecked(False)
        self.crushed.setChecked(False)
        self.mixed_feed.setChecked(False)
        self.document_merge.setChecked(False)
        self.auto_orient.setEnabled(False)
        self.auto_crop.setEnabled(False)
        self.image_enhancement.setEnabled(False)
        self.document_merge_adf_flatbed.setEnabled(False)
        self.multi_pick.setEnabled(False)
        self.batch_seperation.setEnabled(False)
        self.deskew_image.setEnabled(False)
        self.blank_page.setEnabled(False)
        self.crushed.setEnabled(False)
        self.mixed_feed.setEnabled(False)
        self.document_merge.setEnabled(False)      
            
    def DisableAll(self):
        if self.crushed.isChecked() == True or self.mixed_feed.isChecked() == True or self.document_merge.isChecked() == True:
            self.auto_orient.setChecked(False)
            self.auto_crop.setChecked(False)
            self.image_enhancement.setChecked(False)
            self.document_merge_adf_flatbed.setChecked(False)
            self.multi_pick.setChecked(False)
            self.batch_seperation.setChecked(False)
            self.deskew_image.setChecked(False)
            self.blank_page.setChecked(False)
            self.auto_orient.setEnabled(False)
            self.auto_crop.setEnabled(False)
            self.image_enhancement.setEnabled(False)
            self.document_merge_adf_flatbed.setEnabled(False)
            self.multi_pick.setEnabled(False)
            self.batch_seperation.setEnabled(False)
            self.deskew_image.setEnabled(False)
            self.blank_page.setEnabled(False)
        else:
            self.auto_orient.setChecked(False)
            self.auto_crop.setChecked(False)
            self.image_enhancement.setChecked(False)
            self.document_merge_adf_flatbed.setChecked(False)
            self.multi_pick.setChecked(False)
            self.batch_seperation.setChecked(False)
            self.deskew_image.setChecked(False)
            self.blank_page.setChecked(False)
            if self.auto_orient_pri == True:
                self.auto_orient.setEnabled(True)
            if self.auto_crop_pri == True:
                self.auto_crop.setEnabled(True)
            if self.image_enhancement_pri == True:
                self.image_enhancement.setEnabled(True)
            if self.document_merge_adf_flatbed_pri == True:
                self.document_merge_adf_flatbed.setEnabled(True)
            if self.multi_pick_pri == True:
                self.multi_pick.setEnabled(True)
            #if self.batch_seperation_pri == True:
            self.batch_seperation.setEnabled(True)
            if self.deskew_image_pri == True:
                self.deskew_image.setEnabled(True)
            if self.blank_page_pri == True:
                self.blank_page.setEnabled(True)
            
    def Crushed(self):
        if self.crushed.isChecked() == True:
            if self.crushed_pri == True:	    
                self.crushed.setEnabled(True)
            self.comboBox_Color.setCurrentIndex(1)
            self.comboBox_Color.setEnabled(False)
            self.DisableAll()
            self.mixed_feed.setChecked(False)
            self.document_merge.setChecked(False)
            self.mixed_feed.setEnabled(False)
            self.document_merge.setEnabled(False)
        else:
            self.comboBox_Color.setEnabled(True)
            self.DisableAll()
            self.mixed_feed.setChecked(False)
            self.document_merge.setChecked(False)
            if self.mixed_feed_pri == True:
                self.mixed_feed.setEnabled(True)
            if self.document_merge_pri == True:
                self.document_merge.setEnabled(True)
            

    #def SearchablePDF(self):
        #if self.searchablePDF.isChecked() == True:
            #self.searchablePDF.setEnabled(True)


    def Auto_crop(self):
        if self.auto_crop.isChecked() == True:
            if self.auto_crop_pri == True:
                self.auto_crop.setEnabled(True)
            self.CheckEnable()
        else:
            self.CheckEnable()
            
    def Deskew_image(self):
        if self.deskew_image.isChecked() == True:
            if self.deskew_image_pri == True:
                self.deskew_image.setEnabled(True)
            self.CheckEnable()
        else:
            self.CheckEnable()
            
    def Blank_page(self):
        if self.blank_page.isChecked() == True:
            if self.blank_page_pri == True:
                self.blank_page.setEnabled(True)
            self.CheckEnable()
        else:
            self.CheckEnable()

    def Mixed_feed(self):
        if self.mixed_feed.isChecked() == True:
            if self.mixed_feed_pri == True:
                self.mixed_feed.setEnabled(True)
            self.DisableAll()
            self.document_merge.setChecked(False)
            self.crushed.setChecked(False)
            self.document_merge.setEnabled(False)
            self.crushed.setEnabled(False)
            self.comboBox_Papersize.setEnabled(False)
        else:
            self.document_merge.setChecked(False)
            self.DisableAll()
            self.crushed.setChecked(False)
            if self.document_merge_pri == True:
                self.document_merge.setEnabled(True)
            if self.crushed_pri == True:
                self.crushed.setEnabled(True)
            if self.mixed_feed_pri == True:
                self.mixed_feed.setEnabled(True)
            self.comboBox_Papersize.setEnabled(True)
   
    def Document_merge(self):
        if self.document_merge.isChecked() == True:
            if self.document_merge_pri == True:
                self.document_merge.setEnabled(True)
            self.DisableAll()
            self.mixed_feed.setChecked(False)
            self.crushed.setChecked(False)
            self.mixed_feed.setEnabled(False)
            self.crushed.setEnabled(False)
            name = re.search(r'_5000_', self.device_uri)
            if name:
                self.comboBox_Flatbed.setCurrentIndex(0)
            else:
                self.comboBox_Flatbed.setCurrentIndex(2)
            self.comboBox_Flatbed.setEnabled(False)
            self.source = str(self.comboBox_Flatbed.currentText()).lower()
        else:
            self.DisableAll()
            self.mixed_feed.setChecked(False)
            self.crushed.setChecked(False)
            if self.mixed_feed_pri == True:
                self.mixed_feed.setEnabled(True)
            if self.crushed_pri == True:
                self.crushed.setEnabled(True)
            self.comboBox_Flatbed.setEnabled(True)
            self.comboBox_Flatbed.setCurrentIndex(1)
            self.source = str(self.comboBox_Flatbed.currentText()).lower()

    def Document_merge_adf_flatbed(self):
        if self.document_merge_adf_flatbed.isChecked() == True:
            self.CheckEnable()
            if self.document_merge_adf_flatbed_pri == True:
                self.document_merge_adf_flatbed.setEnabled(True)
            self.comboBox_Type.setCurrentIndex(2)
            self.comboBox_Type.setEnabled(False) 
            self.pushButton_Merge.setEnabled(True)
        else:
            self.CheckEnable()
            self.pushButton_Merge.setEnabled(False)
            self.comboBox_Type.setCurrentIndex(0)
            self.comboBox_Type.setEnabled(True)
 
 
            
    def Image_enhancement(self):
        if self.image_enhancement.isChecked() == True:
            self.CheckEnable()
            if self.image_enhancement_pri == True:
                self.image_enhancement.setEnabled(True)
            self.s1.setEnabled(True)
            self.s2.setEnabled(True)
            self.s3.setEnabled(True)
            self.s4.setEnabled(True)
        else:
            #self.image_enhancement.setEnabled(False)
            self.CheckEnable()	 
            self.s1.setEnabled(False)
            self.s2.setEnabled(False)
            self.s3.setEnabled(False)
            self.s4.setEnabled(False)

    def mergeButton_clicked(self):
        from PyPDF2 import PdfFileReader, PdfFileMerger
        path1 = str(path)
        #print path1
        output_pdf = utils.createSequencedFilename("Merged", ".pdf",path1)
        files = [ f for f in os.listdir(path1)  if f.startswith('hpscanMerge') and f.endswith('.pdf')]
        if((len(files)) != 0):
            files.sort()  
            merger = PdfFileMerger()
            os.chdir(path1)
            #print os.getcwd()
            for x in files:
                merger.append(PdfFileReader(x), 'hpscan')
            for p in files:
                os.remove(p)

            merger.write(output_pdf)
            imageprocessing.merge_PDF_viewer(output_pdf)
            '''pdf_viewer = ''
            pdf_viewer_list = ['kpdf', 'acroread', 'xpdf', 'evince',]
            for v in pdf_viewer_list:
                vv = utils.which(v)
                if vv:
                    pdf_viewer = os.path.join(vv, v)
                    break
            #cmd = "%s %s &" % (pdf_viewer, output_pdf)
            cmd = pdf_viewer + "  " + output_pdf + " " + "&"
            print cmd               
            os_utils.execute(cmd)
            #sys.exit(0)'''
        else:
            self.warningMessage(no_pages_to_merge)
             
    def change_source(self):
        #device_name = re.search(r'_5000_', self.device_uri)
        #if device_name:
        if re.search(r'_5000_', self.device_uri):
            self.device_name = '5000'
        elif re.search(r'_7500', self.device_uri):
            self.device_name = '7500'
        #print (self.device_uri)
        if self.device_name == '7500' or self.device_name == '5000':
            self.comboBox_SourceChanged(self.device_name)
        
        
    def comboBox_device_URI(self):
        self.device_uri = str(self.comboBox_Device_URI.currentText())
        self.change_source()
        

    def retranslateUi(self, HpScan):
        HpScan.setWindowTitle(_translate("HpScan", "HP-Scan", None))
        self.label_Type.setText(_translate("HpScan", "    Type", None))
        self.label_Size.setText(_translate("HpScan", "    Size", None))
        self.label_Path.setText(_translate("HpScan", "    Path", None))
        self.label_Device.setText(_translate("HpScan", "    Device", None))
        #self.label_Color.setText(_translate("HpScan", "    Color", None))
        self.label_Brightness.setText(_translate("HpScan", "    Brightness", None))
        self.label_Contrast.setText(_translate("HpScan", "    Contrast", None))
        self.label_Sharpness.setText(_translate("HpScan", "    Sharpness", None))
        self.label_Color_value.setText(_translate("HpScan", "    Color", None))	
        self.comboBox_Type.setItemText(0, _translate("HpScan", "PNG", None))
        self.comboBox_Type.setItemText(1, _translate("HpScan", "JPG", None))
        self.comboBox_Type.setItemText(2, _translate("HpScan", "PDF", None))
        self.comboBox_Type.setItemText(3, _translate("HpScan", "TIFF", None))
        '''self.comboBox_Flatbed.setItemText(0, _translate("HpScan", "Flatbed", None))
        self.comboBox_Flatbed.setItemText(1, _translate("HpScan", "ADF", None))
        self.comboBox_Flatbed.setItemText(2, _translate("HpScan", "Duplex", None))
        self.comboBox_Flatbed.setCurrentIndex(1)'''
        #self.comboBox_Color.setItemText(0, _translate("HpScan", "Lineart", None))
        self.comboBox_Color.setItemText(0, _translate("HpScan", "Gray", None))
        self.comboBox_Color.setItemText(1, _translate("HpScan", "Color", None))
        self.comboBox_Color.setCurrentIndex(0)
        self.comboBox_Resolution.setItemText(0, _translate("HpScan", "75", None))
        self.comboBox_Resolution.setItemText(1, _translate("HpScan", "100", None))
        self.comboBox_Resolution.setItemText(2, _translate("HpScan", "200", None))
        self.comboBox_Resolution.setItemText(3, _translate("HpScan", "300", None))
        self.comboBox_Resolution.setItemText(4, _translate("HpScan", "600", None))
        self.comboBox_Resolution.setCurrentIndex(3)
        #self.comboBox_Papersize.setItemText(0, _translate("HpScan", "5x7", None))
        #self.comboBox_Papersize.setItemText(1, _translate("HpScan", "4x6", None))
        #self.comboBox_Papersize.setItemText(2, _translate("HpScan", "3x5", None))
        #self.comboBox_Papersize.setItemText(3, _translate("HpScan", "a2_env", None))
        #self.comboBox_Papersize.setItemText(4, _translate("HpScan", "a3", None))
        self.comboBox_Papersize.setItemText(0, _translate("HpScan", "a4", None))
        self.comboBox_Papersize.setItemText(1, _translate("HpScan", "a5", None))
        #self.comboBox_Papersize.setItemText(7, _translate("HpScan", "a6", None))
        #self.comboBox_Papersize.setItemText(8, _translate("HpScan", "b4", None))
        self.comboBox_Papersize.setItemText(2, _translate("HpScan", "b5", None))
        #self.comboBox_Papersize.setItemText(10, _translate("HpScan", "c6_env", None))
        #self.comboBox_Papersize.setItemText(11, _translate("HpScan", "dl_env", None))
        #self.comboBox_Papersize.setItemText(12, _translate("HpScan", "exec", None))
        #self.comboBox_Papersize.setItemText(13, _translate("HpScan", "flsa", None))
        #self.comboBox_Papersize.setItemText(14, _translate("HpScan", "higaki", None))
        #self.comboBox_Papersize.setItemText(15, _translate("HpScan", "japan_env_3", None))
        #self.comboBox_Papersize.setItemText(16, _translate("HpScan", "japan_env_4", None))
        self.comboBox_Papersize.setItemText(3, _translate("HpScan", "legal", None))
        self.comboBox_Papersize.setItemText(4, _translate("HpScan", "letter", None))
        #self.comboBox_Papersize.setItemText(19, _translate("HpScan", "no_10_env", None))
        #self.comboBox_Papersize.setItemText(20, _translate("HpScan", "oufufu-hagaki", None))
        #self.comboBox_Papersize.setItemText(21, _translate("HpScan", "photo", None))
        #self.comboBox_Papersize.setItemText(22, _translate("HpScan", "super_b", None))
        #self.comboBox_Papersize.setItemText(23, _translate("HpScan", "b6", None))
        self.comboBox_Papersize.setCurrentIndex(4)        
        self.pushButton_Scan.setText(_translate("HpScan", "Scan", None))
        
        self.pushButton_Change.setText(_translate("HpScan", "Change Path", None))
        self.pushButton_Merge.setText(_translate("HpScan", "Merge", None))
        self.auto_orient.setText(_translate("HpScan", "Auto Orient ", None))
        self.crushed.setText(_translate("HpScan", "Crushed page scan ", None))
        #self.searchablePDF.setText(_translate("HpScan", "Searchable PDF ", None))
        self.auto_crop.setText(_translate("HpScan", "Crop to content on page ", None))
        self.deskew_image.setText(_translate("HpScan", "Straighten page content ", None))
        self.multi_pick.setText(_translate("HpScan", "Misfeed(multipick) detection", None))
        self.blank_page.setText(_translate("HpScan", "Delete blank pages ", None))
        self.batch_seperation.setText(_translate("HpScan", "Separate the document", None))
        self.bp_blankpage.setText(_translate("HpScan", "Before each blank page", None))
        self.bp_barcode.setText(_translate("HpScan", "Before each page with a barcode", None))
        self.comboBox_Barcode_Type.setItemText(0, _translate("HpScan", "Any format", None))
        self.comboBox_Barcode_Type.setItemText(1, _translate("HpScan", "Code 39,Code 39 full ASCII", None))
        self.comboBox_Barcode_Type.setItemText(2, _translate("HpScan", "EAN 8/13,UPC-a,UPC-E(6-digit)", None))
        self.comboBox_Barcode_Type.setItemText(3, _translate("HpScan", "Code 128,GS1-128(UCC/EAN-128)", None))
        self.comboBox_Barcode_Type.setItemText(4, _translate("HpScan", "Codebar", None))
        self.comboBox_Barcode_Type.setItemText(5, _translate("HpScan", "ITF(2 of 5 interleaved)", None))
        self.comboBox_Barcode_Type.setItemText(6, _translate("HpScan", "PDF 417", None))
        self.comboBox_Barcode_Type.setItemText(7, _translate("HpScan", "Postnet code", None))
        self.document_merge.setText(_translate("HpScan", "Page merge", None))
        self.document_merge_adf_flatbed.setText(_translate("HpScan", "Document merge", None))
        self.image_enhancement.setText(_translate("HpScan", "Image enhancement", None))        
        self.mixed_feed.setText(_translate("HpScan", "Mixed document feed", None))
       
        i = 0
        #print self.devicelist
        for device in self.devicelist:
            if re.search(r'_5000_', device) or re.search(r'_7500', device):
                self.comboBox_Device_URI.addItem(device)
                self.comboBox_Device_URI.setItemText(i, _translate("HpScan", device, None))
                i += 1
            else:
                self.other_device_cnt += 1
        self.comboBox_path.addItem(path)
        self.comboBox_path.setItemText(0,_translate("HpScan", path, None))    
        
        
        #self.pushButton_Cancel.setText(_translate("HpScan", "Cancel", None))


class SetupDialog():
    #print ("calling ui5 scan.py")
    def setupUi(self):
        #scanjet_flag=''
        #print ("called ui5 scan.py")
        #list1=[]
        import sys
        app = QtWidgets.QApplication(sys.argv)
        #app = QApplication(sys.argv)
        HpScan = QtWidgets.QDockWidget()
        ui = Ui_HpScan()

        devicelist = {}
        #device = ''
        sane.init()
        sane_devices = sane.getDevices()
        for d, mfg, mdl, t in sane_devices:
            try:
                devicelist[d]
            except KeyError:
                devicelist[d] = [mdl]
            else:
                devicelist[d].append(mdl)
        sane.deInit()
        #print (devicelist)

        ui.devicelist = devicelist
        #print ui.devicelist

        ui.setupUi(HpScan)

        scanjet_flag=imageprocessing.check_pil()
        if scanjet_flag is not None:
            #ui.DisableAllScanjet()
            ui.auto_orient.setEnabled(False)
            ui.auto_crop.setEnabled(False)
            ui.image_enhancement.setEnabled(False)
            ui.document_merge_adf_flatbed.setEnabled(False)
            ui.multi_pick.setEnabled(False)
            #ui.batch_seperation.setEnabled(False)
            ui.deskew_image.setEnabled(False)
            ui.blank_page.setEnabled(False)
            ui.crushed.setEnabled(False)
            ui.mixed_feed.setEnabled(False)
            ui.document_merge.setEnabled(False)        

            ui.deskew_image_pri = False
            ui.auto_crop_pri = False
            ui.mixed_feed_pri = False
            ui.auto_orient_pri = False
            ui.document_merge_adf_flatbed_pri = False
            #ui.multi_pick_pri = False
            #ui.batch_seperation_pri = False
            ui.crushed_pri = False
            ui.document_merge_pri = False
            ui.image_enhancement_pri = False
            ui.blank_page_pri = False

        scanjet_flag=imageprocessing.check_numpy()
        if scanjet_flag is not None:
            #ui.DisableAllScanjet()
            ui.auto_orient.setEnabled(False)
            ui.auto_crop.setEnabled(False)
            #ui.image_enhancement.setEnabled(False)
            ui.document_merge_adf_flatbed.setEnabled(False)
            ui.multi_pick.setEnabled(False)
            #ui.batch_seperation.setEnabled(False)
            ui.deskew_image.setEnabled(False)
            #ui.blank_page.setEnabled(False)
            ui.crushed.setEnabled(False)
            ui.mixed_feed.setEnabled(False)
            ui.document_merge.setEnabled(False)   


            ui.blank_page.setEnabled(True)
            #ui.blank_page.setChecked(False)
            ui.image_enhancement.setEnabled(True)
            #ui.image_enhancement.setChecked(False)

            ui.deskew_image_pri = False
            ui.auto_crop_pri = False
            ui.mixed_feed_pri = False
            ui.auto_orient_pri = False
            ui.document_merge_adf_flatbed_pri = False
            #ui.multi_pick_pri = False
            #ui.batch_seperation_pri = False
            ui.crushed_pri = False
            ui.document_merge_pri = False

        scanjet_flag=imageprocessing.check_opencv()
        if scanjet_flag is not None:
            ui.deskew_image.setEnabled(False)
            ui.auto_crop.setEnabled(False)
            ui.mixed_feed.setEnabled(False)
            #ui.deskew_image.setChecked(False)
            #ui.auto_crop.setChecked(False)
            #ui.mixed_feed.setChecked(False)

            ui.deskew_image_pri = False
            ui.auto_crop_pri = False
            ui.mixed_feed_pri = False

        scanjet_flag=imageprocessing.check_tesserocr_imutils()
        if scanjet_flag is not None:
            ui.auto_orient.setEnabled(False)
            #ui.auto_orient.setChecked(False)

            ui.auto_orient_pri = False

        scanjet_flag=imageprocessing.check_pypdf2()
        if scanjet_flag is not None:
            ui.document_merge.setEnabled(False)
            ui.document_merge_adf_flatbed.setEnabled(False)
            #ui.document_merge.setChecked(False)

            ui.document_merge_pri = False
            ui.document_merge_adf_flatbed_pri = False

        scanjet_flag=imageprocessing.check_zbar()
        #print scanjet_flag
        if scanjet_flag is not None:
            #print "setting barcode to false"
            ui.bp_barcode.setEnabled(False)

            ui.batchsepBC_pri = False

        #list1.append(HpScan)
        #list1.append(app)
        #print HpScan
        '''list_scanjet=imageprocessing.validate_scanjet_support()
        print (list_scanjet)
        if(list_scanjet[2] == 'False'):
            scanjet_error="Scanjet features are not supported and disabled for %s %s. Please upgrade to latest distro version"% (list_scanjet[0],list_scanjet[1])
            ui.failureMessage(scanjet_error)
            ui.DisableAllScanjet()
        source_option = device.getOptionObj("source").constraint
        print (source_option)
        valid_res = device.getOptionObj('resolution').constraint
        print (valid_res)
        available_scan_mode = device.getOptionObj("mode").constraint
        print (available_scan_mode)'''
        #print ui.comboBox_Device_URI.count()
        if ui.comboBox_Device_URI.count() == 0:
            if ui.other_device_cnt > 0:
                scanjet_error="hp-uiscan is not supported for this device"
                ui.failureMessage(scanjet_error)
            else:
                scanjet_error="No device connected"
                ui.failureMessage(scanjet_error)
        else:
            HpScan.show()
            sys.exit(app.exec_())
        #return list1

