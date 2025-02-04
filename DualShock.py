import pygame

pygame.init() # initializes all the pygame modules
running = True

# initialize controller(with joysticks)
joysticks = []
for i in range(pygame.joystick.get_count()):
    joysticks.append(pygame.joystick.Joystick(i))
for joystick in joysticks:
    joystick.init()

# Maps analog keys
# 0: L_Horizontal, 1: L_Vertical (-1 -> 1, 0 as starting point)
# 2: R_Horizontal, 3: R_Vertical (-1 -> 1, 0 as starting point)
# 4: L_Trigger, 5: R_Trigger (-1 -> 1, -1 as starting point)
analog_keys = {0:0, 1:0, 2:0, 3:0, 4:-1, 5:-1}
formated_analog_keys = {} # Dictionary

digital_keys = [] # Digital key holder, list
digital_key_mapping = {
                0: "X",
                1: "○",
                2: "□",
                3: "△",
                4: "Share",
                5: "PS",
                6: "Option",
                7: "Lclick",
                8: "Rclick",
                9: "L1",
                10: "R1",
                11: "↑",
                12: "↓",
                13: "←",
                14: "→",
                15: "Touchpad"
            } #Digital key map

while running:
    # Check User Input
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.JOYBUTTONDOWN: # Check if digital button is held down
             key = digital_key_mapping.get(event.button)
             if key and key not in digital_keys:
                 digital_keys.append(key)
        elif event.type == pygame.JOYBUTTONUP:
            key = digital_key_mapping.get(event.button)
            if key in digital_keys:
                digital_keys.remove(key)
        
        if event.type == pygame.JOYAXISMOTION:  # Check if analog buttons were used (very sensitive)
            analog_keys[event.axis] = event.value

            # Update formated_analog_keys only if any axis change is greater than 0.1
            if any(abs(analog_keys[axis] - formated_analog_keys.get(axis, 0)) > 0.1 for axis in analog_keys):
                formated_analog_keys = {k: 0 if abs(v) < 0.1 else round(v, 5) for k, v in analog_keys.items()}
            # any() returns true when at least one condition inside it true
            # abs() returns the absolute value
            # for axis in analog_keys, just goes element by element in the analog_keys list

            # {k: ... for k, v in ...items()}, dictionary comprehension, iterates throw each value pair (k and v)
                # k represents each axis, v is the value
            # if abs(v) < 0.1 else round(v, 5), if the current value is smaller then .1 set to zero (creates a dead zone)

        # Output digital key and analog values to the terminal
        print(f"\r{digital_keys}\t{formated_analog_keys}", end= "", flush=True)