microStep_list = [1/16, 1/8, 1/4, 1/2, 1]
limitSteps = 5

def getSteps(usableSteps: int, stepValue: int):

    #Caso base: No se considera ningún step pero tampoco se necesita ningun valor
    if(usableSteps == 0 and stepValue == 0):
        return True
    
    #Caso base: No se considera ningún step pero se necesita un valor
    if(usableSteps == 0 and stepValue != 0):
        return False
    
    #Caso base: No se necesita ningún step
    if(stepValue == 0):
        return True
    
    #Caso recursivo: El step considerado es muy grande
    if(microStep_list[usableSteps-1] > stepValue  and    (usableSteps != 0 and stepValue != 0)):
        return getSteps(usableSteps - 1, stepValue)
    
    #Caso recursivo: El step considerado si se puede tomar en cuenta
    if(microStep_list[usableSteps-1] <= stepValue  and    (usableSteps != 0 and stepValue != 0)):
        return getSteps(usableSteps - 1, stepValue) or getSteps(usableSteps - 1, stepValue - microStep_list[usableSteps-1]) or getSteps(usableSteps, stepValue - microStep_list[usableSteps])


print(getSteps(len(microStep_list), 3))