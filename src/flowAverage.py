import statistics as s

duty_cycle = 225
flow_file = "240.txt"
lista_flujos = []

def lineSplitter(line: list):
    new_list = [(i.strip('\n')).split(",") for i in line]
    return new_list

with open("./docs/{}".format(flow_file)) as f:
    data_lines = f.readlines()

split_lines = lineSplitter(data_lines)

for i in range(len(split_lines)-1):
    volumen_actual = float(split_lines[i][0])
    tiempo_actual = float(split_lines[i][1])/1000 #De ms a s

    volumen_siguiente = float(split_lines[i+1][0])
    tiempo_siguiente = float(split_lines[i+1][1])/1000 #De ms a s

    #Flujo -> mL/s
    flujo = (volumen_siguiente - volumen_actual)/(tiempo_siguiente - tiempo_actual)
    lista_flujos.append(flujo)


flujo_promedio = s.mean(lista_flujos)

print("Con un duty cycle de {}, el flujo promedio fue de {} mL/s.".format(duty_cycle,flujo_promedio))




