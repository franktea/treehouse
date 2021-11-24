use piston_window::*;
use std::{collections::LinkedList};
use rand::distributions::{Distribution, Uniform};

fn main() {
    let mut window: PistonWindow = WindowSettings::new("Snake", 
    [(BLOCK_SIZE * WIDTH) as u32, (BLOCK_SIZE * HEIGHT) as u32])
    .exit_on_esc(true).build().unwrap();

    let mut world = World::new(); // 创建world

    while let Some(event) = window.next() {
        window.draw_2d(&event, |context, graphics, _device| {
            clear([1.0; 4], graphics);
            world.draw(context, graphics);
        });

        // 键盘输入
        if let Some(Button::Keyboard(key)) = event.press_args() {
            world.key_press(key);
        }

        // 距离上一次update的时间，需要用UpdateEvent来获取
        if let Some(ref args) = event.update_args() {
            world.update(args.dt); // dt就是距离上一次update的时间
        }
    }
}

const WIDTH: i32 = 20; // 窗口宽度，20个方块
const HEIGHT: i32 = 20; // 窗口高度，20个方块
const BLOCK_SIZE: i32 = 40; // 每个方块的边长，40像素

const RED: [f32; 4] = [1.0, 0.0, 0.0, 1.0];
const BLUE: [f32; 4] = [0.0, 0.0, 1.0, 1.0];
const GREEN: [f32; 4] = [0.0, 1.0, 0.0, 1.0];

#[derive(Clone, PartialEq)]
struct Pos {
    x: i32,
    y: i32,
}

impl Pos {
    fn to_world_rect(&self) -> [f64; 4] {
        [(self.x * BLOCK_SIZE) as f64, (self.y * BLOCK_SIZE) as f64, BLOCK_SIZE as f64 - 1.0, BLOCK_SIZE as f64 - 1.0]
    }
}

#[derive(PartialEq)]
enum State {Running, Dead}

fn key_to_direction(key: Key) -> i32 {
    match key {
        Key::Up => 0,
        Key::Right => 1,
        Key::Down => 2,
        Key::Left => 3,
        _ => -1,
    }
}

struct World {
    food: Pos, // 食物的位置
    snake: LinkedList<Pos>, // 蛇的每一个节点的位置
    time_elapsed: f64, // 距离上一次更新状态的时间
    direction: i32, // 蛇的朝向，0: Up, 1: Right, 2: Down, 3: Left
    state: State, // 游戏状态
}

impl World {
    fn new() -> Self {
        let mut l: LinkedList<Pos> = LinkedList::new();
        l.push_back(Pos{x: WIDTH / 2, y: HEIGHT / 2});

        Self {
            food: rand_food(&l), // 随机生成的食物位置
            snake: l,
            time_elapsed: 0.0,
            direction: 0,
            state: State::Running,
        }
    }

    fn draw(&self,  context: Context, g: &mut G2d) {
        // 画蛇
        for (i, pos) in self.snake.iter().enumerate() {
            let c = if i == 0 { BLUE } else { GREEN };
            rectangle(c, pos.to_world_rect(), context.transform, g);
        }

        // 画食物
        rectangle(RED, self.food.to_world_rect(), context.transform, g);
    }

    fn key_press(&mut self, key: Key) {
        if self.state == State::Dead {
            return;
        }

        let dir = key_to_direction(key);
        if dir < 0 || (dir - self.direction).abs() == 2 {
            return; // == 2表示是让蛇掉头，游戏的规则不允许
        }

        self.direction = dir;
        self.move_forward(); // move forward
        self.time_elapsed = 0.0;
    }

    fn move_forward(&mut self) {
        let dx = [0, 1, 0, -1];
        let dy = [-1, 0, 1, 0];
        let new_x = self.snake.front().unwrap().x + dx[self.direction as usize];
        let new_y = self.snake.front().unwrap().y + dy[self.direction as usize];

        // 出界
        if new_x < 0 || new_x >= WIDTH || new_y < 0 || new_y >= HEIGHT {
            self.state = State::Dead;
            return;
        }

        // 撞到自己
        if self.snake.contains(&Pos {x: new_x, y: new_y}) {
            self.state = State::Dead;
            return;
        }

        // 吃到食物
        if new_x == self.food.x && new_y == self.food.y {
            self.snake.push_front(self.food.clone());
            self.food = rand_food(&self.snake);
            return;
        }

        // 正常往前移动一步
        self.snake.push_front(Pos{x: new_x, y: new_y});
        self.snake.pop_back();
    }

    fn update(&mut self, delta_time: f64) {
        if self.state == State::Dead {
            return;
        }

        self.time_elapsed += delta_time;
        if self.time_elapsed >= 1.0 {
            self.move_forward();
            self.time_elapsed = 0.0;
        }
    }
}

// 随机生成一个食物的位置，参数snake是蛇的位置，生成的食物要求不能与蛇所在的全部位置重叠
fn rand_food(snake: &LinkedList<Pos>) -> Pos {
    // 随机数库的用法，照着文档搬过来的
    let mut rng = rand::thread_rng();
    let die_x = Uniform::from(0..WIDTH);
    let die_y = Uniform::from(0..HEIGHT);

    let mut pos = Pos {x: die_x.sample(&mut rng), y: die_y.sample(&mut rng)};

    while snake.contains(&pos) {
        pos = Pos {x: die_x.sample(&mut rng), y: die_y.sample(&mut rng)};
    }

    return pos;
}