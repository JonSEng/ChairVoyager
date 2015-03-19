import urllib2
import webbrowser
import csv

asia1 = "http://www.npr.org/2015/03/13/392718510/indias-prime-minister-makes-a-swing-through-indian-ocean-nations"
asia2 = "http://www.npr.org/blogs/codeswitch/2015/03/16/393284680/in-hawaii-a-wwii-internment-camp-named-national-monument"
europe1 = "http://www.npr.org/blogs/thesalt/2015/03/11/392355408/the-fate-of-the-worlds-chocolate-depends-on-this-spot-in-rural-england"
europe2 = "http://www.npr.org/2015/03/16/393284652/in-eastern-ukraine-a-cease-fire-in-name-only"
latinamerica1 = "http://www.npr.org/2015/03/16/393284592/brazilians-stage-massive-protests-against-president-dilma-rousseff"
latinamerica2 = "http://www.npr.org/2015/03/17/393530800/in-mexico-a-fight-over-press-freedom"
middleeast1 = "http://www.npr.org/2015/03/15/392365713/state-of-terror-where-isis-came-from-and-how-to-fight-it"
middleeast2 = "http://www.npr.org/blogs/thetwo-way/2015/03/17/393539858/israels-election-netanyahu-seeks-fourth-term"
articleURLs = [asia1, asia2, europe1, europe2, latinamerica1, latinamerica2, middleeast1, middleeast2]

i=0
oldIndex = -1
while (i < 8):
	response = urllib2.urlopen('https://data.sparkfun.com/output/Jx3n8mWoYEsNwYYDwWM0.csv')
	f = open('csvData.csv', 'wb')
	f.write(response.read())
	f.close()
	csvFile = csv.reader(open('csvData.csv', 'rb'),delimiter=",")
	garbage = csvFile.next()
	mostRecent = csvFile.next()[0]
	index = int(mostRecent)
	if(index != oldIndex):
		print("just tried to open" + articleURLs[index])
		webbrowser.open(articleURLs[index])
		oldIndex = index
		i += 1