import pygame
import pandas as pd

# Initialize Pygame
pygame.init()

# Define colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
COLOR_MAP = {
    0: BLACK,
    1: WHITE,  # NEST
    2: (255, 165, 0),  # FOOD (Orange)
    3: (0, 255, 0),  # CHEMICAL (Green)
    4: (0, 0, 255),  # WORKER_WITH_FOOD (Blue)
    5: (255, 0, 0),  # WORKER_WITHOUT_FOOD (Red)
}

# Set dimensions
dimension = 100
cell_size = 5  # Adjust as needed

# Set screen dimensions
screen_width = dimension * cell_size
screen_height = dimension * cell_size
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Animated Display")

# Read the CSV file into a DataFrame
df = pd.read_csv('result.csv', index_col=0)

# Function to update the display
def update_display(frame):
    screen.fill(WHITE)  # Clear the screen
    index = 0
    current_df = df.loc[[frame]]
    for i in range(dimension):
        for j in range(dimension):
            char_value = current_df.iloc[0, index]
            
            color = COLOR_MAP[char_value]
            pygame.draw.rect(screen, color, (j * cell_size, i * cell_size, cell_size, cell_size))
            index += 1
    # Extract total food collected
    total_food_collected = current_df.iloc[0, -1]
    font = pygame.font.SysFont(None, 25)
    text = font.render(f'Total Food Collected: {total_food_collected}', True, WHITE)
    screen.blit(text, (10, 10))

# Main loop
running = True
frame = 0
clock = pygame.time.Clock()
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    update_display(frame)
    pygame.display.flip()
    frame = (frame + 1) % 500  # Assuming 500 frames
    clock.tick(10)  # Adjust as needed for the desired frame rate

pygame.quit()
