bits = 10
mid = 1.5

while True:
    analog = input('value to convert:')
    digital = ''
    compare = mid
    step = mid/2
    for b in xrange(bits):
        if analog > compare:
            digital += '1'
            compare += step
        else:
            digital += '0'
            compare -= step
        step = 0.5*step
    hexformat = hex(int(digital, 2))
    print str(analog) + ' to digital: ' + str(hexformat)
    print ''
