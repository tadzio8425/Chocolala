
def getSteps(desiredStep: float):

    full_steps = 0
    half_steps = 0
    quarter_steps = 0
    eight_steps = 0
    sixteenth_steps = 0


    step_partitions = desiredStep / 0.0625
    
    full_steps = step_partitions//16
    decimalStep = (desiredStep) % 1

    if decimalStep >= 0.5:
        decimalStep -= 0.5
        half_steps += 1

    if decimalStep >= 0.25:
        decimalStep -= 0.25
        quarter_steps += 1

    if decimalStep >= 0.125:
        decimalStep -= 0.125
        eight_steps += 1

    if decimalStep >= 0.0625:
        decimalStep -= 0.0625
        sixteenth_steps += 1
        
    return [full_steps, half_steps, quarter_steps, eight_steps, sixteenth_steps]

print(getSteps(1))


