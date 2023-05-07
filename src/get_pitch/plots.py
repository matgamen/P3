import matplotlib.pyplot as plt
import numpy as np
import wave

fileName = "P3\prueba.wav"
spf = wave.open(fileName, "r")

# Extract Raw Audio from Wav File
signal = spf.readframes(-1)
signal = np.fromstring(signal, "int16")
startFonema = 15400
endFonema = 16000
fonema = signal[startFonema:endFonema] #segon de 0.77 a 0.8
tempsFonema = range(startFonema, endFonema)
senyalFonema = np.column_stack((fonema, tempsFonema))


longAutocorrelation = len(fonema)*2
autocorrelation = [0]
mostra1 = 0
mostra2 = 0
tmp = 0


for m in range(longAutocorrelation):
    tmp = 0
    mostra1 = 0
    mostra2 = 0
    if m<len(fonema):
        for n in range(m):
            mostra1 = int(fonema[n])
            mostra2 = int(fonema[len(fonema) - (m - n)])
            tmp += mostra1*mostra2
    else:
        for n in range(len(fonema)-(m-len(fonema))):
            #print(m,n)
            mostra1 = int(fonema[len(fonema)-(m-len(fonema))-n-1])
            mostra2 = int(fonema[len(fonema)-n-1])
            tmp += mostra1 * mostra2
    
    autocorrelation.insert(m,tmp)

valor_norm = autocorrelation[600]

#Normalitzem el senyal d'autocorrelació
for x in range(longAutocorrelation):
    autocorrelation[x] = autocorrelation[x] / valor_norm  

fig, axs = plt.subplots(2)
fig.suptitle('senyal sonora i autocorrelació')
axs[0].plot(fonema)
axs[1].plot(autocorrelation)
plt.show()