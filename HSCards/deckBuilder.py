import sys
import time
import pyautogui


def typeDeckTitle():
	x, y = posDeckTitle
	pyautogui.moveTo(x, y)
	pyautogui.click()

	if(timeSleep>0):
		time.sleep(timeSleep)

	pyautogui.typewrite(deckTitle)
	pyautogui.press('enter')


def typeCard(cardName):
	x, y = posSearch
	pyautogui.moveTo(x, y)
	pyautogui.click()

	if(timeSleep>0):
		time.sleep(timeSleep)

	pyautogui.typewrite(cardName)
	pyautogui.press('enter')


def addCards(num):
	x, y = posCard1
	pyautogui.moveTo(x, y)
	pyautogui.click()

	if num != 1:
		pyautogui.click()


posCard1 = int(sys.argv[1]), int(sys.argv[2])
posCard2 = int(sys.argv[3]), int(sys.argv[4])
posSearch = int(sys.argv[5]), int(sys.argv[6])
posDeckTitle = int(sys.argv[7]), int(sys.argv[8])
deckTitle = sys.argv[9]
timeSleep = float(sys.argv[10])
deckArgv = sys.argv[11:]
deckNames = []
deckNums = []

for i in range(len(deckArgv)):
	if i%2 == 0:
		deckNames.append(deckArgv[i])
	else:
		deckNums.append(deckArgv[i])

print("PosCard1: ", posCard1)
print("PosCard2: ", posCard2)
print("PosSearch: ", posSearch)
print("PosDeckTitle: ", posDeckTitle)
print("DeckTitle: ", deckTitle)
print("TimeSleep: ", timeSleep)
print("DeckNames: ", deckNames)
print("DeckNums: ", deckNums)

typeDeckTitle()

for i in range(len(deckNames)):
	typeCard(deckNames[i])
	addCards(int(deckNums[i]))
	
