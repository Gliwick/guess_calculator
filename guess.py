def dif_to_points(dif):
	return (dif * dif - 15 * dif + 58) // 2;

def legend_points(points, num):
	if num <= 8:
		return points >= 203
	if num in {9, 10}:
		return points >= 200
	if num in {11, 12}:
		return points >= 190
	if num >= 13:
		return points >= 180
	return False

def pokemon_points(points, num):
	if num <= 8:
		return points >= 190
	if num in {9, 10}:
		return points >= 180
	if num in {11, 12}:
		return points >= 165
	if num >= 13:
		return points >= 150
	return False

def points_to_money(prizes, points, num):
	if num <= 8:
		if points >= 203:
			return prizes[0]
		if points >= 190:
			return prizes[1]
		if points >= 175:
			return prizes[2]
		if points >= 165:
			return prizes[3]
		if points >= 155:
			return prizes[4]
	if num in {9, 10}:
		if points >= 200:
			return prizes[0]
		if points >= 180:
			return prizes[1]
		if points >= 160:
			return prizes[2]
		if points >= 150:
			return prizes[3]
		if points >= 140:
			return prizes[4]
	if num in {11, 12}:
		if points >= 190:
			return prizes[0]
		if points >= 165:
			return prizes[1]
		if points >= 140:
			return prizes[2]
		if points >= 130:
			return prizes[3]
		if points >= 120:
			return prizes[4]
	if num >= 13:
		if points >= 180:
			return prizes[0]
		if points >= 150:
			return prizes[1]
		if points >= 120:
			return prizes[2]
		if points >= 110:
			return prizes[3]
		if points >= 100:
			return prizes[4]
	return 0

def print_prize_list(prize, winners):
	if winners:
		print('%s: %s' % (prize, ', '.join(sorted(winners))))


tours = dict()
tours_list = []

guess_file = open("data/guess.txt")
guesses = []
for guess in guess_file:
	guesses.append(guess.rstrip().split('\t'))
	if len(guesses[-1]) > 1 and guesses[-1][1] not in tours:
		tours[guesses[-1][1]] = dict()
		tours_list.append(guesses[-1][1])
guess_file.close()

winner_file = open("data/winners.txt")
for name in tours_list:
	tours[name]['num'] = int(winner_file.readline().rstrip())
	tours[name]['winner'] = dict()
	for i in range(min(8, tours[name]['num'])):
		s = winner_file.readline()
		if '\t' in s:
			winner_name = s.split('\t')[1]
		else:
			winner_name = s
		tours[name]['winner'][winner_name.rstrip()] = i + 1
winner_file.close()

late_file = open("data/late.txt")
late = set()
for s in late_file:
	if len(s.rstrip()) > 0:
		late.add(s.rstrip())
late_file.close()

shift = dict()
points = dict()

guessed_winner = {'all': set()}
for tour in tours:
	guessed_winner[tour] = set()

for guess in guesses:
	if len(guess) >= 4:
		user = guess[0].rstrip()
		tour = guess[1]
		place = int(guess[2])
		trainer = guess[3].rstrip()
		if user in points:
			if tour not in points[user]:
				points[user][tour] = 0
				shift[user][tour] = 0
		else:
			points[user] = dict()
			points[user][tour] = 0
			shift[user] = dict()
			shift[user][tour] = 0

		if trainer in late:
			shift[user][tour] += 1
		elif trainer in tours[tour]['winner']:
			place -= shift[user][tour]
			if place <= 8:
				if trainer in {'',}:
					points[user][tour] += max(dif_to_points(abs(tours[tour]['winner'][trainer] - 6)), dif_to_points(abs(tours[tour]['winner'][trainer] - 7)))
				else:
					points[user][tour] += dif_to_points(abs(tours[tour]['winner'][trainer] - place))
			if tours[tour]['winner'][trainer] == 1 and place == 1:
				guessed_winner[tour].add(user)

print(tours_list, tours)

for tour in tours:
	print(tour)
	for user in points:
		if tour in points[user]:
			print(user, points[user][tour])

prize_file = open("data/prizes.txt")
prizes = {'candy': []}
for s in prize_file:
	num = int(s.split('. ')[0])
	s = ' ('.join(s.split('. ')[1].split(' (')[:-1])
	if num == 1:
		jackpot = s.split(' + ')
		prizes['legend'] = ' + '.join(jackpot[:-1])
		currency = ' '.join(jackpot[-1].split(' ')[1:])
		gain = int(jackpot[-1].split(' ')[0])
		prizes[currency] = [gain, 0, 0, 0, 0]
	elif num in {2, 3, 4, 5}:
		lot = s.split(' + ')
		for prize in lot:
			if num == 2 and prize == lot[0]:
				prizes['pokemon'] = prize
			else:
				currency = ' '.join(prize.split(' ')[1:])
				gain = int(prize.split(' ')[0])
				if currency not in prizes:
					prizes[currency] = [0, 0, 0, 0, 0]
				prizes[currency][num - 1] = gain

	elif num in {6, 7}:
		prizes['candy'].append(s.split(' (')[0])
prize_file.close()
if len(prizes['candy']) == 1:
	prizes['candy'].append(prizes['candy'][0])

legend = set()
pokemon = set()
money = dict()
for currency in prizes:
	if type(prizes[currency]) is list and currency != 'candy':
		money[currency] = dict()
for user in points:
	for tour in points[user]:
		if legend_points(points[user][tour], tours[tour]['num']):
			legend.add(user)
		elif pokemon_points(points[user][tour], tours[tour]['num']):
			pokemon.add(user)
		for currency in money:
			if user not in money[currency]:
				money[currency][user] = 0
			money[currency][user] += points_to_money(prizes[currency], points[user][tour], tours[tour]['num'])

money_list = dict()
for currency in money:
	money_list[currency] = dict()
	for user in money[currency]:
		if money[currency][user] > 0:
			if money[currency][user] in money_list[currency]:
				money_list[currency][money[currency][user]].append(user)
			else:
				money_list[currency][money[currency][user]] = [user]

print_prize_list(prizes['legend'], legend)
print_prize_list(prizes['pokemon'], pokemon - legend)

if len(tours) > 1:
	for user in points:
		guessed_winners = 0
		for tour in tours:
			if user in guessed_winner[tour]:
				guessed_winners += 1
		if guessed_winners == len(tours):
			guessed_winner['all'].add(user)
	print_prize_list(prizes['candy'][0] + ' (все лиги)', guessed_winner['all'])

for tour in tours:
	name = ''
	if len(tour.split(' - ')) > 1:
		name = '(%s)' % tour.split(' - ')[1] 
	print_prize_list(' '.join([prizes['candy'][1], name]), guessed_winner[tour] - guessed_winner['all'])

currency_list = sorted(list(money_list))
for currency in currency_list:
	money_list[currency] = sorted([(gain, money_list[currency][gain]) for gain in money_list[currency]], key=lambda x: -x[0])
	for i in range(len(money_list[currency])):
		print_prize_list('%d %s' % (money_list[currency][i][0], currency), money_list[currency][i][1])

