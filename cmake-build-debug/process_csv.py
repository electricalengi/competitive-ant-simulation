import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib.animation import FuncAnimation

# Read the CSV file into a DataFrame
df = pd.read_csv('result.csv', index_col=0)

df = df.drop(columns=["Unnamed: 10001"])
print(df)

dimension = 100
display = [[0 for x in range(dimension)] for y in range(dimension)]

# Create a function to update the display for each frame
def update(frame):
    plt.clf()  # Clear the previous plot
    index = 0
    current_df = df.loc[[frame]]
    for i in range(dimension):
        for j in range(dimension):
            display[i][j] = current_df.iloc[0, index]
            index = index + 1
    plt.imshow(display)

# Create an animation
fig, ax = plt.subplots()
ani = FuncAnimation(fig, update, frames=500, repeat=False)  # 10 frames for 10 ticks

plt.show()
