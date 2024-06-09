import pandas as pd
import matplotlib.pyplot as plt
import os

path = "data/"
files = os.listdir(path)
for _ in files:
    file = pd.read_csv(f"data/{_}", header=None)
    file.columns = ["Gy", "Gy_Filtered", "Speed"]
    title = _.replace("_200", "").replace(".csv", "")
    plt.figure(figsize=(10,5))
    plt.plot(file.index, file["Gy"])
    plt.plot(file.index, file["Gy_Filtered"])
    plt.plot(file.index, file["Speed"])
    plt.xlabel("Step")
    plt.ylabel("Deg/Sec")
    plt.title(title)
    plt.savefig(f'assets/plt/{title}.png')
    plt.show()

