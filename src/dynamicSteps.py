microStep_list = [1/16, 1/8, 1/4, 1/2, 1]
desiredStep = 5.625

microStep_list = [1/16, 1/8, 1/4, 1/2, 1]

microStep_list = [1/16, 1/8, 1/4, 1/2, 1]

def getSteps(usableSteps: int, stepValue: float) -> bool:
    memo = {}

    def memoizedGetSteps(usableSteps: int, stepValue: float) -> list:
        if (usableSteps, stepValue) in memo:
            return memo[(usableSteps, stepValue)]

        if usableSteps == 0 and stepValue == 0:
            return []

        if usableSteps == 0 and stepValue != 0:
            return None

        if stepValue == 0:
            return []

        if microStep_list[usableSteps-1] > stepValue:
            result = memoizedGetSteps(usableSteps - 1, stepValue)
        else:
            result1 = memoizedGetSteps(usableSteps - 1, stepValue)
            result2 = memoizedGetSteps(usableSteps - 1, stepValue - microStep_list[usableSteps-1])
            result3 = memoizedGetSteps(usableSteps, stepValue - microStep_list[usableSteps-1])
            options = [result for result in [result1, result2, result3] if result is not None]
            if not options:
                result = None
            else:
                result = [microStep_list[usableSteps-1]] + min(options, key=len)

        memo[(usableSteps, stepValue)] = result
        return result

    return memoizedGetSteps(len(microStep_list), stepValue)


steps = getSteps(len(microStep_list), desiredStep)
print(steps)