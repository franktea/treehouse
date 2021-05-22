use rand::distributions::{Distribution, Uniform};
use piston_window::*;

// 代表细胞的矩形的长度和宽度
const CELL_SIZE: i32 = 20;
// 二维矩形的行数
const CELL_ROWS: i32 = 30;
// 二维矩形的列数
const CELL_COLS: i32 = 40;

// 活着的细胞的颜色，黑色 
const ALIVE_COLOR: [f32; 4] = [0.0, 0.0, 0.0, 1.0];
// 死亡细胞的颜色，我用了浅灰色，因为没有画边框，白色的太不明显
const DEAD_COLOR: [f32; 4] = [0.9, 0.9, 0.9, 1.0];

// 每个0.04秒更新一次所有细胞的状态
const FRAME_LENGTH: f64 = 0.04;

// 表示二维矩形的World结构
struct World {
    // 二维bool数组
    cells : [[bool; CELL_COLS as usize]; CELL_ROWS as usize],
    // 距离上一次更新状态的时间，如果超过上面设定的0.04，则触发一次状态更新
    time_elapsed: f64,
}

impl World {
    fn new() -> Self {
        let mut rng = rand::thread_rng();
        let die = Uniform::from(0..2);
        let mut cells: [[bool; CELL_COLS as usize]; CELL_ROWS as usize] = 
            [[false; CELL_COLS as usize]; CELL_ROWS as usize];
        for row in 0..CELL_ROWS {
            for col in 0..CELL_COLS {
                let v = die.sample(&mut rng);
                cells[row as usize][col as usize] = if v == 0 { false } else { true };
            }
        }

        World {
            cells,
            time_elapsed: 0.0,
        }
    }

    fn next(&mut self) {
        let alive_neighbours = |x, y| {
            let deltax: [i32; 8] = [0,  1, 1, 1, 0, -1, -1, -1];
            let deltay: [i32; 8] = [-1, -1, 0, 1, 1,  1,  0, -1];
            let mut ret = 0;
            for i in 0..8usize {
                let x2 = x + deltax[i];
                let y2 = y + deltay[i];
                if x2 >= CELL_COLS || x2 < 0 || y2 >= CELL_ROWS || y2 < 0 {
                    continue;
                }
                ret += if self.cells[y2 as usize][x2 as usize] { 1 } else { 0 };
            }
            ret
        };

        let mut tmp: [[bool; CELL_COLS as usize]; CELL_ROWS as usize] = 
            [[false; CELL_COLS as usize]; CELL_ROWS as usize];
        for row in 0..CELL_ROWS {
            for col in 0..CELL_COLS {
                let nb = alive_neighbours(col, row);
                if nb > 3 || nb < 2 {
                    tmp[row as usize][col as usize] = false;
                } else if nb == 2 {
                    tmp[row as usize][col as usize] = self.cells[row as usize][col as usize];
                } else { // nb = 3
                    tmp[row as usize][col as usize] = true;
                }
            }
        }

        self.cells = tmp;
    }

    fn draw_rects(&self,  context: Context, g: &mut G2d) {
        for row in 0..CELL_ROWS {
            for col in 0..CELL_COLS {
                let color: [f32; 4] = if self.cells[row as usize][col as usize] { ALIVE_COLOR } else { DEAD_COLOR };
                rectangle(color,
                    [(col * CELL_SIZE) as f64 + 1.0,
                        (row * CELL_SIZE) as f64 + 1.0,
                        CELL_SIZE as f64 - 2.0,
                        CELL_SIZE as f64 - 2.0],
                context.transform,
                g);
            }
        }
    }

    fn update(&mut self, delta_time: f64) {
        self.time_elapsed += delta_time;
        if self.time_elapsed >= FRAME_LENGTH {
            self.time_elapsed = 0.0;
            self.next();
        }
    }
}

fn main() {
    let mut window: PistonWindow =
        WindowSettings::new("Game of life with rust", [(CELL_SIZE * CELL_COLS) as u32, (CELL_SIZE * CELL_ROWS) as u32])
        .exit_on_esc(true).build().unwrap();

    let mut world = World::new(); // 创建world

    while let Some(event) = window.next() {
        window.draw_2d(&event, |context, graphics, _device| {
            clear([1.0; 4], graphics);
            world.draw_rects(context, graphics); // 绘制world中的细胞
        });

        // 距离上一次update的时间，需要用UpdateEvent来获取
        if let Some(ref args) = event.update_args() {
            world.update(args.dt); // dt就是距离上一次update的时间
        }
    }
}
