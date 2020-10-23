import sys
import time
import pyautogui

offsetX = 100


def clickOn(x, y, rightClick=False):
	pyautogui.moveTo(x+offsetX, y)
	# time.sleep(1)
	if rightClick:
		pyautogui.rightClick()
	else:
		pyautogui.click()


def typeOn(x, y, text):
	pyautogui.moveTo(x+offsetX, y)
	# time.sleep(1)
	pyautogui.click()
	pyautogui.typewrite(text)
	pyautogui.press('enter')
	time.sleep(1)



pyautogui.PAUSE = 0.25
pyautogui.FAILSAFE = True
cardName = sys.argv[1]

try:
	clickOn(1600, 1005)#Atras (ajustado izq)
	time.sleep(2)
	clickOn(1070, 910)#Mi Coleccion
	time.sleep(4)
	typeOn(1000, 992, cardName)#Buscar
	clickOn(1240, 992)#Creacion
	clickOn(1425, 605)#Golden
	clickOn(440, 360, True)#Carta
	time.sleep(2)
	clickOn(1050, 915)#Craft
	clickOn(440, 360)#Fuera
	clickOn(1580, 1000)#Listo (ajustado izq)
	clickOn(1600, 1000)#Atras (ajustado izq)
	time.sleep(1)
	clickOn(980, 570)#Modos
	time.sleep(1)
	clickOn(1180, 500)#Arena
	pyautogui.moveTo(1600+offsetX, 1005)#Colocar en Atras

except pyautogui.FailSafeException:
	print('Abort...\n')
