# -*- coding: utf-8 -*-
#
# (c) Copyright 2001-2015 HP Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# Authors: Don Welch
#

# StdLib
import operator
import signal

# Local
from base.g import *
from base import device, utils
from prnt import cups
from base.codes import *
from .ui_utils import *
from base.sixext import to_unicode
# Qt
from PyQt4.QtCore import *
from PyQt4.QtGui import *

# Ui
from .faxsetupdialog_base import Ui_Dialog
from .deviceuricombobox import DEVICEURICOMBOBOX_TYPE_FAX_ONLY

fax_enabled = prop.fax_build

if fax_enabled:
    try:
        from fax import fax
    except ImportError:
        # This can fail on Python < 2.3 due to the datetime module
        # or if fax was diabled during the build
        fax_enabled = False

if not fax_enabled:
    log.warn("Fax disabled.")

class PasswordDialog(QDialog):
    def __init__(self, prompt, parent=None, name=None, modal=0, fl=0):
        QDialog.__init__(self, parent)
        # Application icon
        self.setWindowIcon(QIcon(load_pixmap('hp_logo', '128x128')))
        self.prompt = prompt

        Layout= QGridLayout(self)
        Layout.setMargin(11)
        Layout.setSpacing(6)

        self.PromptTextLabel = QLabel(self)
        Layout.addWidget(self.PromptTextLabel,0,0,1,3)

        self.UsernameTextLabel = QLabel(self)
        Layout.addWidget(self.UsernameTextLabel,1,0)

        self.UsernameLineEdit = QLineEdit(self)
        self.UsernameLineEdit.setEchoMode(QLineEdit.Normal)
        Layout.addWidget(self.UsernameLineEdit,1,1,1,2)

        self.PasswordTextLabel = QLabel(self)
        Layout.addWidget(self.PasswordTextLabel,2,0)

        self.PasswordLineEdit = QLineEdit(self)
        self.PasswordLineEdit.setEchoMode(QLineEdit.Password)
        Layout.addWidget(self.PasswordLineEdit,2,1,1,2)

        self.OkPushButton = QPushButton(self)
        Layout.addWidget(self.OkPushButton,3,2)

        self.CancelPushButton = QPushButton(self)
        Layout.addWidget(self.CancelPushButton, 3, 1)        

        self.languageChange()

        self.resize(QSize(420,163).expandedTo(self.minimumSizeHint()))

        self.connect(self.OkPushButton, SIGNAL("clicked()"), self.accept)
        self.connect(self.CancelPushButton, SIGNAL("clicked()"), self.reject)
        self.connect(self.PasswordLineEdit, SIGNAL("returnPressed()"), self.accept)

    def setDefaultUsername(self, defUser, allowUsernameEdit = True):
        self.UsernameLineEdit.setText(defUser)
        if not allowUsernameEdit:
            self.UsernameLineEdit.setReadOnly(True)
            self.UsernameLineEdit.setStyleSheet("QLineEdit {background-color: lightgray}")
    
    def getUsername(self):
        return to_unicode(self.UsernameLineEdit.text())


    def getPassword(self):
        return to_unicode(self.PasswordLineEdit.text())


    def languageChange(self):
        self.setWindowTitle(self.__tr("HP Device Manager - Enter Username/Password"))
        self.PromptTextLabel.setText(self.__tr(self.prompt))
        self.UsernameTextLabel.setText(self.__tr("Username:"))
        self.PasswordTextLabel.setText(self.__tr("Password:"))
        self.OkPushButton.setText(self.__tr("OK"))
        self.CancelPushButton.setText(self.__tr("Cancel"))


    def __tr(self,s,c = None):
        return qApp.translate("SetupDialog",s,c)

def showPasswordUI(prompt, userName=None, allowUsernameEdit=True):
    try:
        dlg = PasswordDialog(prompt, None)

        if userName != None:
            dlg.setDefaultUsername(userName, allowUsernameEdit)

        if dlg.exec_() == QDialog.Accepted:
            return (dlg.getUsername(), dlg.getPassword())
        else:
            return ("", "")
    finally:
        pass

    return ("", "")


class FaxSetupDialog(QDialog, Ui_Dialog):
    def __init__(self, parent, device_uri):
        QDialog.__init__(self, parent)
        self.setupUi(self)
        self.device_uri = device_uri
        self.initUi()
        self.dev = None
        self.fax_number = ''
        self.fax_company_name = ''
        self.call_password_ui = True
        self.user_settings = UserSettings()
        self.user_settings.load()
        self.user_settings.debug()

        QTimer.singleShot(0, self.updateUi)


    def initUi(self):
        # connect signals/slots
        self.connect(self.CancelButton, SIGNAL("clicked()"), self.CancelButton_clicked)
        self.connect(self.CancelBSaveBtnutton, SIGNAL("clicked()"), self.SaveBtn_Clicked)
        self.connect(self.FaxComboBox, SIGNAL("DeviceUriComboBox_noDevices"), self.FaxComboBox_noDevices)
        self.connect(self.FaxComboBox, SIGNAL("DeviceUriComboBox_currentChanged"), self.FaxComboBox_currentChanged)
        self.FaxComboBox.setType(DEVICEURICOMBOBOX_TYPE_FAX_ONLY)
        signal.signal(signal.SIGINT, signal.SIG_DFL)

        # Application icon
        self.setWindowIcon(QIcon(load_pixmap('hp_logo', '128x128')))

        if self.device_uri:
            self.FaxComboBox.setInitialDevice(self.device_uri)

        self.NameCompanyLineEdit.setMaxLength(50)
        self.FaxNumberLineEdit.setMaxLength(50)
        self.FaxNumberLineEdit.setValidator(PhoneNumValidator(self.FaxNumberLineEdit))
        self.VoiceNumberLineEdit.setMaxLength(50)
        self.VoiceNumberLineEdit.setValidator(PhoneNumValidator(self.VoiceNumberLineEdit))
        self.EmailLineEdit.setMaxLength(50)
        self.fax_number = to_unicode(self.FaxNumberLineEdit.text())
        self.fax_company_name = to_unicode(self.NameCompanyLineEdit.text())        
        
        '''
        self.connect(self.NameCompanyLineEdit, SIGNAL("editingFinished()"),
                     self.NameCompanyLineEdit_editingFinished)

        self.connect(self.NameCompanyLineEdit, SIGNAL("textChanged(const QString &)"),
                     self.NameCompanyLineEdit_textChanged)

        self.connect(self.FaxNumberLineEdit, SIGNAL("editingFinished()"),
                     self.FaxNumberLineEdit_editingFinished)

        self.connect(self.FaxNumberLineEdit, SIGNAL("textChanged(const QString &)"),
                     self.FaxNumberLineEdit_textChanged)
        '''
        self.connect(self.VoiceNumberLineEdit, SIGNAL("editingFinished()"),
                     self.VoiceNumberLineEdit_editingFinished)

        self.connect(self.VoiceNumberLineEdit, SIGNAL("textChanged(const QString &)"),
                     self.VoiceNumberLineEdit_textChanged)

        self.connect(self.EmailLineEdit, SIGNAL("editingFinished()"),
                     self.EmailLineEdit_editingFinished)

        self.connect(self.EmailLineEdit, SIGNAL("textChanged(const QString &)"),
                     self.EmailLineEdit_textChanged)
         
        self.connect(self.tabWidget,SIGNAL("currentChanged(int)"),self.Tabs_currentChanged)

        self.name_company_dirty = False
        self.fax_number_dirty = False
        self.voice_number_dirty = False
        self.email_dirty = False


    def updateUi(self):
        if not fax_enabled:
            FailureUI(self, self.__tr("<b>PC send fax support is not enabled.</b><p>Re-install HPLIP with fax support or use the device front panel to send a fax.</p><p>Click <i>OK</i> to exit.</p>"))
            self.close()
            return

        self.FaxComboBox.updateUi()
        self.tabWidget.setCurrentIndex(0)
        self.fax_number = to_unicode(self.FaxNumberLineEdit.text())
        self.fax_company_name = to_unicode(self.NameCompanyLineEdit.text())        


    def FaxComboBox_currentChanged(self, device_uri):
        self.device_uri = device_uri
        self.updateCoverpageTab()

        if self.dev is not None:
            self.dev.close()

        try:
            self.dev = fax.getFaxDevice(self.device_uri)
        except Error:
            CheckDeviceUI(self)
            return

        self.updateHeaderTab()



    def FaxComboBox_noDevices(self):
        FailureUI(self, self.__tr("<b>No devices that require fax setup found.</b>"))
        self.close()

    #
    # Name/Company (for TTI header) (stored in device)
    #

    def NameCompanyLineEdit_editingFinished(self):
        self.saveNameCompany(to_unicode(self.NameCompanyLineEdit.text()))


    def NameCompanyLineEdit_textChanged(self, s):
        self.name_company_dirty = True


    def saveNameCompany(self, s):
        self.name_company_dirty = False
        retn = False
        beginWaitCursor()
        try:
            try:
                log.debug("Saving station name %s to device" % s)
                if self.dev.isAuthRequired() == True and self.call_password_ui == True:
                    promptText = "Enter the printer's username password password\n"                    
                    while(True):
                        username, password = showPasswordUI(promptText)
                        if username == '' or password == '':
                            return False
                        respCode = self.dev.getCDMToken(username, password)
                        if respCode != 200:
                            promptText = "Invalid Username or Password!.\nRernter the printer's username password password\n"
                            continue
                        break                
                self.dev.setStationName(s)
                retn = True
            except Error:
                CheckDeviceUI(self)
        finally:
            endWaitCursor()
            return retn

    #
    # Fax Number (for TTI header) (stored in device)
    #

    def FaxNumberLineEdit_editingFinished(self):
        self.saveFaxNumber(to_unicode(self.FaxNumberLineEdit.text()))


    def FaxNumberLineEdit_textChanged(self, s):
        self.fax_number_dirty = True


    def saveFaxNumber(self, s):
        self.fax_number_dirty = False
        retn = False
        beginWaitCursor()
        try:
            try:
                log.debug("Saving fax number %s to device" % s)
                if self.dev.isAuthRequired() == True and self.call_password_ui == True:
                    promptText = "Enter the printer's username password password\n"
                    while(True):
                        username, password = showPasswordUI(promptText)
                        if username == '' or password == '':
                            return retn
                        respCode = self.dev.getCDMToken(username, password)
                        if respCode != 200:
                            promptText = "Invalid Username or Password!.\nRernter the printer's username password password\n"
                            continue
                        break
                self.dev.setPhoneNum(s)
                retn=True
            except Error:
                CheckDeviceUI(self)
        finally:
            endWaitCursor()
            return retn

    #
    # Voice Number (for coverpage) (stored in ~/.hplip/hplip.conf)
    #

    def VoiceNumberLineEdit_editingFinished(self):
        self.saveVoiceNumber(to_unicode(self.VoiceNumberLineEdit.text()))


    def VoiceNumberLineEdit_textChanged(self, s):
        self.voice_number_dirty = True


    def saveVoiceNumber(self, s):
        log.debug("Saving voice number (%s) to ~/.hplip/hplip.conf" % s)
        self.voice_number_dirty = False
        #user_conf.set('fax', 'voice_phone', s)
        self.user_settings.voice_phone = s
        self.user_settings.save()

    #
    # EMail (for coverpage) (stored in ~/.hplip/hplip.conf)
    #

    def EmailLineEdit_editingFinished(self):
        self.saveEmail(to_unicode(self.EmailLineEdit.text()))


    def EmailLineEdit_textChanged(self, s):
        self.email_dirty = True


    def saveEmail(self, s):
        log.debug("Saving email address (%s) to ~/.hplip/hplip.conf" % s)
        self.email_dirty = False
        #user_conf.set('fax', 'email_address', s)
        self.user_settings.email_address = s
        self.user_settings.save()

    #
    #
    #

    def CancelButton_clicked(self):
        self.close()

    def SaveBtn_Clicked(self):

        current_fax_num = self.fax_number = to_unicode(self.FaxNumberLineEdit.text())
        current_fax_company = to_unicode(self.NameCompanyLineEdit.text())
        
        if current_fax_num != self.fax_number:            
            if self.saveFaxNumber(self.fax_number) == False:
                self.FaxNumberLineEdit.setText(current_fax_num)
            else:
                self.fax_number = to_unicode(self.FaxNumberLineEdit.text())
            self.call_password_ui = False

        if  current_fax_company != self.fax_company_name:            
            if self.saveNameCompany(self.fax_company_name) == False:
                self.NameCompanyLineEdit.setText(current_fax_company)
            else:
                self.fax_company_name = to_unicode(self.NameCompanyLineEdit.text())
            self.call_password_ui = False

    def Tabs_currentChanged(self, tab=0):
        """ Called when the active tab changes.
            Update newly displayed tab.
        """        
        if tab == 0:
            self.updateHeaderTab()
        elif tab ==1:    
            self.updateCoverpageTab()
            

    def updateHeaderTab(self):
        beginWaitCursor()
        try:
            try:
                name_company = to_unicode(self.dev.getStationName())
                log.debug("name_company = '%s'" % name_company)
                self.NameCompanyLineEdit.setText(name_company)
                fax_number = str(self.dev.getPhoneNum())
                log.debug("fax_number = '%s'" % fax_number)
                self.FaxNumberLineEdit.setText(fax_number)
            except Error:
                CheckDeviceUI(self)
        finally:
            endWaitCursor()


    def updateCoverpageTab(self):
        #voice_phone = user_conf.get('fax', 'voice_phone')
        voice_phone = self.user_settings.voice_phone
        log.debug("voice_phone = '%s'" % voice_phone)
        self.VoiceNumberLineEdit.setText(voice_phone)
        #email_address = user_conf.get('fax', 'email_address')
        email_address = self.user_settings.email_address
        log.debug("email_address = '%s'" % email_address)
        self.EmailLineEdit.setText(email_address)


    def closeEvent(self, e):
        if self.voice_number_dirty:
            self.VoiceNumberLineEdit.emit(SIGNAL("editingFinished()"))

        if self.name_company_dirty:
            self.NameCompanyLineEdit.emit(SIGNAL("editingFinished()"))

        if self.email_dirty:
            self.EmailLineEdit.emit(SIGNAL("editingFinished()"))

        if self.fax_number_dirty:
            self.FaxNumberLineEdit.emit(SIGNAL("editingFinished()"))

        if self.dev is not None:
            self.dev.close()

        e.accept()

    #
    # Misc
    #

    def __tr(self,s,c = None):
        return qApp.translate("FaxSetupDialog",s,c)


