#!/Users/conn/homebrew/bin/python3

# This is a version of leds_py that works for Python 3 since
# dict iteration and division works differently

# led matrix key: https://docs.google.com/spreadsheets/d/1lbGaP5_LLMeVbfyhL-n2jLwjyEv8ifzQ3fmifeANX_U/edit

# Here are some "for instance" examples of leds based on groupings modify this to your needs
pattern = range(88,120)
blue = [34,35,36,37,38,51,52,53,54,55,65,66,68]
red = [39,40,41,69]
purple = [73,74,75,81]
green = [13,14,15,16,31,32,33,48,49,50,83]

registers = [0,0,0,0]

# Now using the "for instance" arrays, putting them into a dict. Again, modify this based on your needs.
# However recommend doing it as a dict so the output is easier to parse, even if you only have one array
all_keys = {"pattern":pattern, "blue":blue, "red":red, "purple":purple, "green":green}

#Loop through all_keys to do the calculation
for key_group_name, key_group_keys in all_keys.items():
    for led in key_group_keys:
        nled = led -1
        id = nled // 32
        element = nled % 32
        value = 2 ** element
        registers[id] = registers[id] + value

    print('%s .id0 = %s, .id1 = %s, .id2 = %s, .id3 = %s,'%(key_group_name, registers[0], registers[1], registers[2], registers[3]))
    registers = [0,0,0,0]

