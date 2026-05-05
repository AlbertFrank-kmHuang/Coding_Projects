import pygame
import random
import sys
from collections import deque

# ---------- 游戏配置 ----------
WIDTH, HEIGHT = 16, 16          # 网格尺寸（列，行）
CELL_SIZE = 32                  # 像素
SCREEN_SIZE = (WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE)
TOTAL_MINES = 40                # 雷数

# 颜色
COLOR_BG = (192, 192, 192)              # 未翻开背景
COLOR_REVEALED = (224, 224, 224)        # 翻开背景
COLOR_NUMBER = (0, 0, 0)                # 数字颜色
COLOR_FLAG = (255, 0, 0)                # 旗子颜色
COLOR_MINE = (0, 0, 0)                  # 雷的颜色
COLOR_LINES = (128, 128, 128)           # 网格线

# ---------- 游戏状态 ----------
class CellState:
    HIDDEN = 0
    REVEALED = 1
    FLAGGED = 2

class Minesweeper:
    def __init__(self):
        self.reset()

    def reset(self):
        self.grid = [[{'mine': False, 'adj': 0, 'state': CellState.HIDDEN}
                      for _ in range(WIDTH)] for _ in range(HEIGHT)]
        # 布雷
        placed = 0
        while placed < TOTAL_MINES:
            x = random.randint(0, WIDTH - 1)
            y = random.randint(0, HEIGHT - 1)
            if not self.grid[y][x]['mine']:
                self.grid[y][x]['mine'] = True
                placed += 1

        # 计算邻接雷数
        for y in range(HEIGHT):
            for x in range(WIDTH):
                if self.grid[y][x]['mine']:
                    self.grid[y][x]['adj'] = -1
                    continue
                cnt = 0
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        if dx == 0 and dy == 0: continue
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < WIDTH and 0 <= ny < HEIGHT and self.grid[ny][nx]['mine']:
                            cnt += 1
                self.grid[y][x]['adj'] = cnt
        self.game_over = False
        self.win = False

    def get_cell(self, x, y):
        return self.grid[y][x]

    # 翻开一个格子，返回是否踩雷
    def reveal(self, x, y):
        if not (0 <= x < WIDTH and 0 <= y < HEIGHT):
            return False
        cell = self.grid[y][x]
        if self.game_over or cell['state'] == CellState.REVEALED or cell['state'] == CellState.FLAGGED:
            return False
        if cell['mine']:
            cell['state'] = CellState.REVEALED
            self.game_over = True
            self.win = False
            return True

        # BFS 展开空白区域
        q = deque()
        q.append((x, y))
        while q:
            cx, cy = q.popleft()
            cur = self.grid[cy][cx]
            if cur['state'] != CellState.HIDDEN:
                continue
            cur['state'] = CellState.REVEALED

            if cur['adj'] == 0:
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        if dx == 0 and dy == 0: continue
                        nx, ny = cx + dx, cy + dy
                        if 0 <= nx < WIDTH and 0 <= ny < HEIGHT:
                            nb = self.grid[ny][nx]
                            if nb['state'] == CellState.HIDDEN and not nb['mine']:
                                q.append((nx, ny))
        self.check_win()
        return False

    # 自动展开周围（数字格周围旗数等于雷数时）
    def auto_expand(self, x, y):
        if not (0 <= x < WIDTH and 0 <= y < HEIGHT):
            return
        if self.game_over:
            return
        cell = self.grid[y][x]
        if cell['state'] != CellState.REVEALED or cell['mine']:
            return

        # 统计周围旗子数
        flag_cnt = 0
        neighbors = []
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if dx == 0 and dy == 0: continue
                nx, ny = x + dx, y + dy
                if 0 <= nx < WIDTH and 0 <= ny < HEIGHT:
                    nb = self.grid[ny][nx]
                    if nb['state'] == CellState.FLAGGED:
                        flag_cnt += 1
                    neighbors.append((nx, ny))

        if flag_cnt == cell['adj']:
            for nx, ny in neighbors:
                nb = self.grid[ny][nx]
                if nb['state'] == CellState.HIDDEN:
                    if nb['mine']:
                        nb['state'] = CellState.REVEALED
                        self.game_over = True
                        self.win = False
                        return
                    else:
                        self.reveal(nx, ny)
        self.check_win()

    def toggle_flag(self, x, y):
        if not (0 <= x < WIDTH and 0 <= y < HEIGHT):
            return
        if self.game_over:
            return
        cell = self.grid[y][x]
        if cell['state'] == CellState.REVEALED:
            return
        if cell['state'] == CellState.FLAGGED:
            cell['state'] = CellState.HIDDEN
        else:
            cell['state'] = CellState.FLAGGED

    def check_win(self):
        for y in range(HEIGHT):
            for x in range(WIDTH):
                cell = self.grid[y][x]
                if not cell['mine'] and cell['state'] != CellState.REVEALED:
                    return
        self.game_over = True
        self.win = True

# ---------- 渲染 ----------
def draw_cell(screen, x, y, cell):
    rect = pygame.Rect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1)
    if cell['state'] == CellState.REVEALED:
        color = COLOR_REVEALED
    else:
        color = COLOR_BG
    pygame.draw.rect(screen, color, rect)
    pygame.draw.rect(screen, COLOR_LINES, rect, 1)

    if cell['state'] == CellState.FLAGGED:
        cx = x * CELL_SIZE + CELL_SIZE // 2
        cy = y * CELL_SIZE + CELL_SIZE // 2
        pygame.draw.line(screen, COLOR_FLAG, (cx - 8, cy - 8), (cx - 8, cy + 8), 2)
        pygame.draw.polygon(screen, COLOR_FLAG, [(cx - 8, cy - 8), (cx, cy - 4), (cx - 8, cy)])
    elif cell['state'] == CellState.REVEALED:
        if cell['mine']:
            center = (x * CELL_SIZE + CELL_SIZE // 2, y * CELL_SIZE + CELL_SIZE // 2)
            pygame.draw.circle(screen, COLOR_MINE, center, CELL_SIZE // 4)
        elif cell['adj'] > 0:
            font = pygame.font.Font(None, CELL_SIZE // 2 + 8)
            text = font.render(str(cell['adj']), True, COLOR_NUMBER)
            text_rect = text.get_rect(center=(x * CELL_SIZE + CELL_SIZE // 2, y * CELL_SIZE + CELL_SIZE // 2))
            screen.blit(text, text_rect)

def draw_game_over(screen, win):
    overlay = pygame.Surface(SCREEN_SIZE, pygame.SRCALPHA)
    overlay.fill((0, 0, 0, 180))
    screen.blit(overlay, (0, 0))
    font = pygame.font.Font(None, 36)
    if win:
        msg = "You Win! Press R to restart."
    else:
        msg = "Game Over! Press R to restart."
    text = font.render(msg, True, (255, 255, 255))
    text_rect = text.get_rect(center=(SCREEN_SIZE[0] // 2, SCREEN_SIZE[1] // 2))
    screen.blit(text, text_rect)

# ---------- 主程序 ----------
def main():
    pygame.init()
    screen = pygame.display.set_mode(SCREEN_SIZE)
    pygame.display.set_caption("Minesweeper - Left:Reveal  Right:Flag / 1:Reveal 2:Flag / R:Restart")
    clock = pygame.time.Clock()

    game = Minesweeper()
    running = True

    # 控制鼠标点击冷却 (避免快速连点导致多次触发)
    mouse_cooldown = 0

    while running:
        # 事件处理（主要处理鼠标按钮和键盘 R）
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_r:
                    game.reset()
                    mouse_cooldown = 0

        # 获取鼠标位置
        mouse_x, mouse_y = pygame.mouse.get_pos()
        grid_x = mouse_x // CELL_SIZE
        grid_y = mouse_y // CELL_SIZE
        valid = 0 <= grid_x < WIDTH and 0 <= grid_y < HEIGHT

        # 鼠标按键检测（带冷却）
        if not game.game_over and valid and mouse_cooldown <= 0:
            left = pygame.mouse.get_pressed()[0]
            right = pygame.mouse.get_pressed()[2]

            if left:
                cell = game.get_cell(grid_x, grid_y)
                if cell['state'] == CellState.HIDDEN:
                    game.reveal(grid_x, grid_y)
                elif cell['state'] == CellState.REVEALED:
                    game.auto_expand(grid_x, grid_y)
                mouse_cooldown = 8   # 约 8 帧（60fps 下约 130ms）
            elif right:
                cell = game.get_cell(grid_x, grid_y)
                if cell['state'] != CellState.REVEALED:
                    game.toggle_flag(grid_x, grid_y)
                else:
                    # 右键在已翻开格子上同样触发快速清扫
                    game.auto_expand(grid_x, grid_y)
                mouse_cooldown = 8

        # 键盘 1、2 键检测（保留，同样带冷却）
        keys = pygame.key.get_pressed()
        if not game.game_over and valid and mouse_cooldown <= 0:
            if keys[pygame.K_1]:
                cell = game.get_cell(grid_x, grid_y)
                if cell['state'] == CellState.HIDDEN:
                    game.reveal(grid_x, grid_y)
                elif cell['state'] == CellState.REVEALED:
                    game.auto_expand(grid_x, grid_y)
                mouse_cooldown = 8
            elif keys[pygame.K_2]:
                cell = game.get_cell(grid_x, grid_y)
                if cell['state'] != CellState.REVEALED:
                    game.toggle_flag(grid_x, grid_y)
                else:
                    game.auto_expand(grid_x, grid_y)
                mouse_cooldown = 8

        # 减少冷却计数器
        if mouse_cooldown > 0:
            mouse_cooldown -= 1

        # 绘制
        screen.fill(COLOR_BG)
        for y in range(HEIGHT):
            for x in range(WIDTH):
                draw_cell(screen, x, y, game.get_cell(x, y))

        if game.game_over:
            draw_game_over(screen, game.win)

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()
