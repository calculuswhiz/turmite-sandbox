import sys

from PyQt5 import uic, QtCore
from PyQt5.QtCore import Qt, QObject, pyqtSignal
from PyQt5.QtWidgets import (QWidget, QApplication,
        QGridLayout, QMainWindow, QTableWidgetItem,
        QLabel, QPushButton, QLineEdit, QSpinBox, QMessageBox)

import AutomatonGenerator

class MW(QMainWindow, AutomatonGenerator.Ui_MainWindow):
    def __init__(self, parent=None):
        super(MW, self).__init__(parent)
        self.setupUi(self)
        self.initwin()
        
    def initwin(self):
        self.move(50, 50)
        
        self.addStateBtn.clicked.connect(self.addState_fn)
        self.remStateBtn.clicked.connect(self.remState_fn)
        self.generateBtn.clicked.connect(self.generate_fn)
        
    def addState_fn(self):
        nextOff = self.nextOffSpinbox.value()
        nextOn  = self.nextOnSpinbox.value()
        turnOff = self.turnOffCombo.currentText()[0]
        turnOn  = self.turnOnCombo.currentText()[0]
        paintOff = int(self.paintOffCombo.currentText() == "On")
        paintOn  = int(self.paintOnCombo.currentText() == "On")
        
        # Add a row
        oldRows = self.stateTable.rowCount()
        self.stateTable.setRowCount(oldRows+1)
        self.stateTable.setItem(oldRows, 0, QTableWidgetItem(str(oldRows)))
        self.stateTable.setItem(oldRows, 1, QTableWidgetItem(str(nextOff)))
        self.stateTable.setItem(oldRows, 2, QTableWidgetItem(str(nextOn)))
        self.stateTable.setItem(oldRows, 3, QTableWidgetItem(str(turnOff)))
        self.stateTable.setItem(oldRows, 4, QTableWidgetItem(str(turnOn)))
        self.stateTable.setItem(oldRows, 5, QTableWidgetItem(str(paintOff)))
        self.stateTable.setItem(oldRows, 6, QTableWidgetItem(str(paintOn)))
        
    def remState_fn(self):
        self.stateTable.setRowCount(self.stateTable.rowCount()-1)
        
    def generate_fn(self):
        if self.stateTable.rowCount() == 0:
            print("Please, make sure there's at least one state.", file=sys.stderr)
            return
        
        print("# Generated Autamaton code")
        print("[Automaton]")
        print("# Name:")
        print("[Name]", '"'+str(self.namebox.text())+'"')
        print()
        print("# Color: r g b")
        print("[Color]", self.redSpinbox.value(), self.greenSpinbox.value(), self.blueSpinbox.value())
        print()
        print("# Start: x y state# orientation")
        print("[Start]", self.startXBox.text(), self.startYBox.text(), self.startStateBox.text(), self.orientCombo.currentText())
        print()
        print("# States: offnext onnext offturn onturn offpaint onpaint")
        
        for i in range(0, self.stateTable.rowCount()):
            print("[State]", self.stateTable.item(i, 1).text(), end=" ")
            print(self.stateTable.item(i, 2).text(), end=" ")
            print(self.stateTable.item(i, 3).text(), end=" ")
            print(self.stateTable.item(i, 4).text(), end=" ")
            print(self.stateTable.item(i, 5).text(), end=" ")
            print(self.stateTable.item(i, 6).text())
            
        print("[/Automaton]")
        print()
        
        # msgBox = QMessageBox()
        # msgBox.setText("Generated code:")
        # msgBox.setDetailedText("Hello")
        # msgBox.exec_()
        
    def keyPressEvent(self, e):
        if e.key() == Qt.Key_Escape:
            self.close()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    Mainwin = MW()
    Mainwin.show()
    
    sys.exit(app.exec_())
