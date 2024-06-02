# Example file showing a circle moving on screen
import pygame
import serial

# pygame setup
window_width = 400
window_height = 400
background_color = (255, 255, 255)
pygame.init()
screen = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption('Radio')
clock = pygame.time.Clock()
running = True
dt = 0
pygame.mixer.init()

# arduino com setup
use_arduino = True
if use_arduino: 
    arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)

# sounds
freq_padding = 5 # size of "clear" buffer
freq_fading = 10 # size of "noisy" buffer

noise = {"file" : "sounds/switchsradio/bruitblanc.mp3", "sound" : None}

#radio sounds
sounds = []
sounds.append({"file" : "sounds/autresfrequences/SINATRA.mp3", "freq" : 544, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/RUSSIANSONG.mp3", "freq" : 1000, "sound" : None})
sounds.append({"file" : "sounds/storytelling/MESSAGE1.mp3", "freq" : 820, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/WAR_OF_WORLDS.mp3", "freq" : 675, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/CHURCHILL.mp3", "freq" : 280, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/PEARL_HARBOR.mp3", "freq" : 433, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/AD.mp3", "freq" : 393, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/RADIOLONDRES.mp3", "freq" : 67, "sound" : None})
sounds.append({"file" : "sounds/autresfrequences/RUSSE.mp3", "freq" : 175, "sound" : None})
sounds.append({"file" : "sounds/storytelling/MESSAGE2.mp3", "id" : "message2", "sound" : None})

channel0 = pygame.mixer.Channel(0) # noise channel
channel1 = pygame.mixer.Channel(1) # radio channel
channel2 = pygame.mixer.Channel(2) # sound channel
channel0.set_volume(0) # mute channel 0 by default
channel1.set_volume(0) # mute channel 1 by default
channel2.set_volume(1) # channel2 is for fx sound and must be always set to 1.


# radio 
radio_switch = 0
radio_freq = 0
radio_volume = 0
radio_playing = ""

# sound preloading
noise["sound"] = pygame.mixer.Sound(noise["file"])
channel0.play(noise["sound"], -1)
for i in range(0, len(sounds)):
    sounds[i]["sound"] = pygame.mixer.Sound(sounds[i]["file"])


while running:
    # poll for events
    # pygame.QUIT event means the user clicked X to close your window
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # load data from arduino
    data = ""
    if use_arduino:
        data = arduino.readline()
    
    if data.startswith(b'radio -- '):
        data = data.replace(b'radio -- ', b"")
        data_splitted = data.split()
        # todo: upgrade with a regex
        radio_switch = int(str(data_splitted[0]).replace("p:", "").replace("b", "").replace("'", ""))
        radio_volume = int(str(data_splitted[1]).replace("v:", "").replace("b", "").replace("'", ""))
        radio_freq = int(str(data_splitted[2]).replace("f:", "").replace("b", "").replace("'", ""))
        label_text = str(radio_switch)+" "+str(radio_volume)+" "+str(radio_freq)+" "+str(channel2.get_busy())

        # global radio volume
        channel_volume = radio_volume / 100
        channel0.set_volume(channel_volume)
        channel1.set_volume(channel_volume)
        # do not update channel 2.

        if(radio_switch == 1): 
            white_noise = True
            for sound in sounds:
                if not "freq" in sound:
                    continue
                freq_start = sound["freq"] - freq_padding - freq_fading
                freq_end = sound["freq"] + freq_padding + freq_fading
                if freq_start <= radio_freq and freq_end >= radio_freq:
                    white_noise = False
                    if radio_playing != sound["file"]:
                        radio_playing = sound["file"]
                        channel1.stop()
                        channel1.play(sound["sound"], -1)

                    # noise before
                    if radio_freq >= sound["freq"] - freq_padding - freq_fading and radio_freq <= sound["freq"] - freq_padding:
                        fading = (radio_freq - (sound["freq"] - freq_padding - freq_fading)) / (freq_fading)
                        sound["sound"].set_volume(fading)
                        noise["sound"].set_volume(1 - fading)
                        # label_text += " before "+str(fading)
                    elif radio_freq >= sound["freq"] - freq_padding and radio_freq <= sound["freq"] + freq_padding:
                        sound["sound"].set_volume(1)
                        noise["sound"].set_volume(0)
                    elif radio_freq >= sound["freq"] + freq_padding and radio_freq <= sound["freq"] + freq_padding + freq_fading:
                        fading = (radio_freq - (sound["freq"] + freq_padding)) / (freq_fading)
                        sound["sound"].set_volume(1 - fading)
                        noise["sound"].set_volume(fading)
                        # label_text += " after "+str(fading)
            
            if(white_noise):
                radio_playing = ""
                channel1.stop()
                noise["sound"].set_volume(1)
        else: 
            channel0.set_volume(0) # mute channel 0 
            channel1.set_volume(0) # mute channel 1 


    elif data.startswith(b'sound -- '):
        data = data.replace(b'sound -- ', b"")
        soundId = str(data).replace("b", "").replace("'", "").replace("\\r\\n", "")
        label_text = "soundId: "+soundId
        for sound in sounds:
            if not "id" in sound:
                continue
            if sound["id"] == soundId:
                channel2.play(sound["sound"])
                while channel2.get_busy() == True:
                    channel1.set_volume(0)
                    channel0.set_volume(0)
               

    else: 
        label_text = "no data"
    # fill the screen with a color to wipe away anything from last frame
    screen.fill(background_color)
    # add text
    text_font = pygame.font.SysFont("monospace", 30)
    text_color = (0, 0, 0)
    label = text_font.render(label_text, 1, text_color)
    screen.blit(label, (0,0))

    # play sounds


    # flip() the display to put your work on screen
    pygame.display.flip()

    # limits FPS to 60
    # dt is delta time in seconds since last frame, used for framerate-
    # independent physics.
    dt = clock.tick(60) / 1000

pygame.quit()
