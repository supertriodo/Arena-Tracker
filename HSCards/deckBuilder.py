import sys
import pyautogui


def typeDeckTitle():
	x, y = posDeckTitle
	pyautogui.moveTo(x, y)
	pyautogui.click()
	pyautogui.typewrite(deckTitle)
	pyautogui.press('enter')


def typeCard(cardName):
	x, y = posSearch
	pyautogui.moveTo(x, y)
	pyautogui.click()
	pyautogui.typewrite(cardName)
	pyautogui.press('enter')


def addCards(num, golden):
	if num == 1:
		x, y = posCard1
		pyautogui.moveTo(x, y)
		pyautogui.click()
	else:
		if golden:
			x, y = posCard2
			pyautogui.moveTo(x, y)
			pyautogui.click()
			pyautogui.click()

		x, y = posCard1
		pyautogui.moveTo(x, y)
		pyautogui.click()
		pyautogui.click()


posCard1 = int(sys.argv[1]), int(sys.argv[2])
posCard2 = int(sys.argv[3]), int(sys.argv[4])
posSearch = int(sys.argv[5]), int(sys.argv[6])
posDeckTitle = int(sys.argv[7]), int(sys.argv[8])
deckTitle = sys.argv[9]
pyautogui.PAUSE = float(sys.argv[10])
goldenCards = bool(sys.argv[11]=="1")
deckArgv = sys.argv[12:]
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
print("GoldenCards: ", goldenCards)
print("DeckNames: ", deckNames)
print("DeckNums: ", deckNums)


try:
	typeDeckTitle()

	for i in range(len(deckNames)):
		typeCard(deckNames[i])
		addCards(int(deckNums[i]), goldenCards)

except pyautogui.FailSafeException:
	print('Abort...\n')
	
