from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
import nap
from appcontext import AppContext


class StatemachineScene(QGraphicsScene):
    def __init__(self):
        super(StatemachineScene, self).__init__()


class StatemachineView(QGraphicsView):
    def __init__(self):
        super(StatemachineView, self).__init__()


class StatemachineEditor(QWidget):
    def __init__(self, ctx):
        super(StatemachineEditor, self).__init__()
        self.setLayout(QVBoxLayout())
        self.view = StatemachineView()
        self.layout().addWidget(self.view)

    def setModel(self, obj:nap.Object):
        pass