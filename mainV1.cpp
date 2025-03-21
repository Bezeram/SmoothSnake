#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <SFML/Audio.hpp>

sf::Music music;
sf::RenderWindow window(sf::VideoMode(1920, 1080), "Snake!", sf::Style::Close | sf::Style::Titlebar | sf::Style::Fullscreen);
sf::View view(sf::Vector2f(320.f, 180.f), sf::Vector2f(640.f, 360.f));
bool play_music = 1;
bool window_settings = 1;
bool keep_playing = 1;

int main()
{
    if (window_settings)
    {
        window.setVerticalSyncEnabled(true);
        window.setKeyRepeatEnabled(false);
        window.setPosition(sf::Vector2i(-10, -10));
    }

    // music
    if (play_music)
    {
        music.openFromFile("sounds/OneChanceNicecut.ogg");
        music.setVolume(25.f);
        music.setLoop(true);
    }

    music.play();

    // check death
    bool check_death = 1;

    // snake
    int snake_size = 2;

    if (snake_size < 2)
        return 0;

    sf::Vector2f initial_position(310, 170);

    std::vector<sf::RectangleShape> body;
    std::vector<sf::Vector2f> previous_pos;

    body.reserve(snake_size);
    previous_pos.reserve(snake_size);
    sf::Vector2i current_direction(1, 0);
    sf::Vector2i previous_direction;

    sf::Texture texture_snake_corner;
    sf::Texture texture_snake_corner_interpolation;
    sf::Texture texture_snake_line;
    sf::Texture texture_snake_head;

    texture_snake_corner.loadFromFile("images/snake_corner.png");
    texture_snake_line.loadFromFile("images/snake_line.png");
    texture_snake_head.loadFromFile("images/snake_head.png");
{
    sf::RectangleShape temp;
    temp.setPosition(initial_position);
    temp.setSize(sf::Vector2f(20, 20));
    //temp.setFillColor(sf::Color::Green);
    temp.setTexture(&texture_snake_head);
    temp.setOrigin(sf::Vector2f(10.f, 10.f));
    //temp.setTextureRect(sf::IntRect(0, 0, 20, 20));

    previous_pos.emplace_back(temp.getPosition());
    body.emplace_back(temp);

    sf::RectangleShape temp2;
    for (int i = 1; i < snake_size; i++)
    {
        temp2.setPosition(initial_position.x + current_direction.x * 20 * i * -1, initial_position.y + current_direction.y * 20 * i * -1);
        temp2.setSize(sf::Vector2f(20, 20));
        //temp2.setFillColor(sf::Color::Blue);
        temp2.setOrigin(10.f, 10.f);
        previous_pos.emplace_back(temp2.getPosition());

        body.emplace_back(temp2);
    }
}
    // initialise snake
    body[0].setRotation(((body[1].getPosition().x > body[0].getPosition().x) * 180 + 90) * (body[1].getPosition().x != body[0].getPosition().x) + ((body[1].getPosition().x == body[0].getPosition().x)) * (body[1].getPosition().y < body[0].getPosition().y) * 180);

    // initialise tail
    body[snake_size - 1].setRotation(((body[snake_size - 2].getPosition().x > body[snake_size - 1].getPosition().x) * 180 + 90) * (body[snake_size - 2].getPosition().x != body[snake_size - 1].getPosition().x) + ((body[snake_size - 2].getPosition().x == body[snake_size - 1].getPosition().x)) * (body[snake_size - 2].getPosition().y < body[snake_size - 1].getPosition().y) * 180);
    body[snake_size - 1].setTexture(&texture_snake_head);

    // copy of tail, runs behind
    sf::RectangleShape tail_copy(body[snake_size - 1]);

    // corner interpolation magic
    std::vector<sf::RectangleShape> corner_interpolation;
    int corner_interpolation_count = 0;
    sf::RectangleShape corner_interpolation_tail_1(sf::Vector2f(20.f, 20.f));
    sf::RectangleShape corner_interpolation_tail_2(sf::Vector2f(20.f, 20.f));
    corner_interpolation_tail_1.setTexture(&texture_snake_corner);
    corner_interpolation_tail_2.setTexture(&texture_snake_corner);
    corner_interpolation_tail_1.setOrigin(10.f, 10.f);
    corner_interpolation_tail_2.setOrigin(10.f, 10.f);
    float tail_angle_1 = 0;
    float tail_angle_2 = 0;

    // initialise body
    for (int i = 1; i < snake_size - 1; i++)
    {
        if (body[i - 1].getPosition().x == body[i + 1].getPosition().x || body[i - 1].getPosition().y == body[i + 1].getPosition().y)
        {
            // line
            body[i].setRotation((body[i - 1].getPosition().x != body[i + 1].getPosition().x) * 90);
        }
        else
        {
            // corner
            // find the position of body[i - 1]
            if (body[i - 1].getPosition().x == body[i].getPosition().x)
            {
                // body[i - 1] is either up or down
                if (body[i - 1].getPosition().y == body[i].getPosition().y + 20)
                {
                    // body[i - 1] is down
                    // check for body[i + 1]
                    if (body[i + 1].getPosition().x == body[i].getPosition().x + 20)
                    {
                        // corner top-left
                        body[i].setRotation(0);
                        corner_interpolation.emplace_back(body[i]);
                    }
                    else
                    {
                        // corner top-right
                        body[i].setRotation(90);
                        corner_interpolation.emplace_back(body[i]);
                    }
                }
                else
                {
                    // body[i - 1] is up
                    // check for body[i + 1]
                    if (body[i + 1].getPosition().x == body[i].getPosition().x - 20)
                    {
                        // corner bottom-right
                        body[i].setRotation(180);
                        corner_interpolation.emplace_back(body[i]);
                    }
                    else
                    {
                        // corner bottom-left
                        body[i].setRotation(270);
                        corner_interpolation.emplace_back(body[i]);
                    }
                }

                body[i].setRotation(90);

                corner_interpolation[corner_interpolation_count++].setTexture(&texture_snake_corner);
            }
            else
            {
                // body[i + 1] is either up or down
                if (body[i + 1].getPosition().y == body[i].getPosition().y + 20)
                {
                    // body[i + 1] is down
                    // check for body[i - 1]
                    if (body[i - 1].getPosition().x == body[i].getPosition().x + 20)
                    {
                        // corner top-left
                        body[i].setRotation(0);
                        corner_interpolation.emplace_back(body[i]);
                    }
                    else
                    {
                        // corner top-right
                        body[i].setRotation(90);
                        corner_interpolation.emplace_back(body[i]);
                    }

                    body[i].setRotation(90);
                }
                else
                {
                    // body[i + 1] is up
                    // check for body[i - 1]
                    if (body[i - 1].getPosition().x == body[i].getPosition().x - 20)
                    {
                        // corner bottom-right
                        body[i].setRotation(180);
                        corner_interpolation.emplace_back(body[i]);
                    }
                    else
                    {
                        // corner bottom-left
                        body[i].setRotation(270);
                        corner_interpolation.emplace_back(body[i]);
                    }
                }

                body[i].setRotation(0);

                corner_interpolation[corner_interpolation_count++].setTexture(&texture_snake_corner);
            }
        }
    }

    // initialise food
    srand(time(0));
    sf::RectangleShape food(sf::Vector2f(20.f, 20.f));
    sf::Texture texture_food;
    texture_food.loadFromFile("images/food.png");
    food.setTexture(&texture_food);
    food.setOrigin(10.f, 10.f);

    // initial food placement
    while (true)
    {
        bool reset = 0;
        food.setPosition( (rand() % 32) * 20 + 10, (rand() % 18) * 20 + 10);

        for (int i = 0; i < snake_size; i++)
            if (food.getPosition() == body[i].getPosition()/* || food.getPosition().x > body[0].getPosition().x - 10 || food.getPosition().x < body[0].getPosition().x + 10 || food.getPosition().y > body[0].getPosition().y - 10 || food.getPosition().y < body[0].getPosition().y + 10*/)
            {
                reset = 1;
                break;
            }

        if (!reset)
            break;
    }

    sf::Clock _clock;
    double dt = 0.1f;
    double accumulator = 0.0f;
    double total_time = 0.0f;

    // game loop
    while (true)
    {
        double current_time = _clock.restart().asSeconds();
        double frameTime = current_time;

        if (frameTime > 0.5f)
            frameTime = 0.5f;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            /// Update snake

            // reset corner interpolation variables
            corner_interpolation.clear();
            corner_interpolation_count = 0;

            // input
            previous_direction = current_direction;

            for (sf::Event check_event; window.pollEvent(check_event);)
            {
                // quit
                if (check_event.type == check_event.Closed || check_event.key.code == sf::Keyboard::Escape)
                {
                    return 0;
                }

                // reset
                if (check_event.key.code == sf::Keyboard::Q)
                {
                    play_music = 0;
                    window_settings = 0;

                    music.stop();
                    return main();
                }

                if (check_event.key.code == sf::Keyboard::Z)
                {
                    // increase speed
                    dt -= 0.025f;
                    break;
                }

                if (check_event.key.code == sf::Keyboard::X)
                {
                    // decrease speed
                    dt += 0.025f;
                    break;
                }

                if (check_event.type == check_event.KeyPressed)
                {
                    if (check_event.key.code == sf::Keyboard::P)
                    {
                        {
                            music.stop();
                            sf::Clock _delay;

                            while (_delay.getElapsedTime().asSeconds() < 1.f);

                            for (bool ok = 1; ok;)
                            {
                                sf::Event check_unpause;
                                while (window.pollEvent(check_unpause))
                                    if (check_unpause.key.code == sf::Keyboard::P)
                                    {
                                        ok = 0;
                                        break;
                                    }
                            }

                            music.play();
                        }
                    }

                    // shift between fullscreen and windowed mode
                    if (check_event.key.code == sf::Keyboard::E)
                    {
                        if (window.getSize().y == 1080)
                        {
                            window.create(sf::VideoMode(640, 360), "Snake!", sf::Style::Close | sf::Style::Titlebar);

                            window.clear();
                            window.setView(view);

                            window.draw(food);

                            for (int i = 0; i < snake_size; i++)
                                window.draw(body[i]);

                            window.display();

                            {
                                sf::Clock clock_delay;

                                while (clock_delay.getElapsedTime().asSeconds() < 1.f);
                            }
                        }
                        else
                        {
                            window.create(sf::VideoMode(1920, 1080), "Snake!", sf::Style::Fullscreen);

                            window.clear();
                            window.setView(view);

                            window.draw(food);

                            for (int i = 0; i < snake_size; i++)
                                window.draw(body[i]);

                            window.display();

                            {
                                sf::Clock clock_delay;

                                while (clock_delay.getElapsedTime().asSeconds() < 1.f);
                            }
                        }

                        break;
                    }

                    if (current_direction.y != 0)
                    {
                        if ((check_event.key.code == sf::Keyboard::Right && !(check_event.key.code == sf::Keyboard::Up || check_event.key.code == sf::Keyboard::Down)) || (check_event.key.code == sf::Keyboard::D && !(check_event.key.code == sf::Keyboard::W || check_event.key.code == sf::Keyboard::S)))
                        {
                            current_direction.x = 1;
                            current_direction.y = 0;
                            break;
                        }

                        if ((check_event.key.code == sf::Keyboard::Left && !(check_event.key.code == sf::Keyboard::Up || check_event.key.code == sf::Keyboard::Down)) || (check_event.key.code == sf::Keyboard::A && !(check_event.key.code == sf::Keyboard::W || check_event.key.code == sf::Keyboard::S)))
                        {
                            current_direction.x = -1;
                            current_direction.y = 0;
                            break;
                        }

                        break;
                    }

                    if ((check_event.key.code == sf::Keyboard::Up && !(check_event.key.code == sf::Keyboard::Right || check_event.key.code == sf::Keyboard::Left)) || (check_event.key.code == sf::Keyboard::W && !(check_event.key.code == sf::Keyboard::R || check_event.key.code == sf::Keyboard::A)))
                    {
                        current_direction.x = 0;
                        current_direction.y = -1;
                        break;
                    }

                    if ((check_event.key.code == sf::Keyboard::Down && !(check_event.key.code == sf::Keyboard::Right || check_event.key.code == sf::Keyboard::Left)) || (check_event.key.code == sf::Keyboard::S && !(check_event.key.code == sf::Keyboard::D || check_event.key.code == sf::Keyboard::A)))
                    {
                        current_direction.x = 0;
                        current_direction.y = 1;
                        break;
                    }
                }
            }

            // update last positions
            for (int i = 0; i < snake_size; i++)
                previous_pos[i] = body[i].getPosition();

            // moving snake
            for (int i = snake_size - 1; i > 0; i--)
                body[i].setPosition(body[i - 1].getPosition());
            body[0].setPosition( body[0].getPosition().x + current_direction.x * 20, body[0].getPosition().y + current_direction.y * 20 );

            // out of bounds check
            if (check_death)
            {
                if (body[0].getPosition().x < 0 || body[0].getPosition().x > 630 || body[0].getPosition().y < 0 || body[0].getPosition().y > 350)
                {
                    play_music = 0;
                    window_settings = 0;

                    music.stop();

                    if (keep_playing)
                        return main();

                    return 0;
                }
            }

            // death by self check
            if (check_death)
            {
                for (int i = 1; i < snake_size; i++)
                    if (body[0].getPosition() == body[i].getPosition())
                    {
                        play_music = 0;
                        window_settings = 0;

                        music.stop();

                        if (keep_playing)
                            return main();

                        return 0;
                    }
            }

            // tail
            body[snake_size - 1].setRotation(((body[snake_size - 2].getPosition().x > body[snake_size - 1].getPosition().x) * 180 + 90) * (body[snake_size - 2].getPosition().x != body[snake_size - 1].getPosition().x) + ((body[snake_size - 2].getPosition().x == body[snake_size - 1].getPosition().x)) * (body[snake_size - 2].getPosition().y < body[snake_size - 1].getPosition().y) * 180);

            // check food
            sf::RectangleShape temp_shape;
            if (body[0].getPosition() == food.getPosition())
            {
                // add to the snake
                temp_shape.setSize(sf::Vector2f(20, 20));
                temp_shape.setPosition(body[snake_size - 1].getPosition());
                //temp_shape.setFillColor(sf::Color::Blue);
                temp_shape.setOrigin(10.f, 10.f);

                while (true)
                {
                    bool reset = 0;
                    food.setPosition( (rand() % 32) * 20 + 10, (rand() % 18) * 20 + 10);

                    for (int i = 0; i < snake_size; i++)
                        if (food.getPosition() == body[i].getPosition()/* || food.getPosition().x > body[0].getPosition().x - 10 || food.getPosition().x < body[0].getPosition().x + 10 || food.getPosition().y > body[0].getPosition().y - 10 || food.getPosition().y < body[0].getPosition().y + 10*/)
                        {
                            reset = 1;
                            break;
                        }

                    if (!reset)
                        break;
                }

                snake_size ++;
                body.push_back(temp_shape);
                previous_pos.emplace_back(temp_shape.getPosition());
                body[snake_size - 1].setTexture(&texture_snake_head);

                //std::cout << food.getPosition().x << ' ' << food.getPosition().y << '\n';
            }

            // load correct rotations for each body part

            // head
            body[0].setRotation(((body[1].getPosition().x > body[0].getPosition().x) * 180 + 90) * (body[1].getPosition().x != body[0].getPosition().x) + ((body[1].getPosition().x == body[0].getPosition().x)) * (body[1].getPosition().y < body[0].getPosition().y) * 180);

            // corner
            if (snake_size != 2)
            {
                corner_interpolation_tail_2 = corner_interpolation_tail_1;
                tail_angle_2 = tail_angle_1;
            }
            else
            {
                if (body[0].getPosition().x != previous_pos[1].x && body[0].getPosition().y != previous_pos[1].y)
                {
                    //std::cout << "nice";
                    // get the correct rotation based on the tail's position and current direction of the head
                    corner_interpolation_tail_2.setPosition(body[1].getPosition());

                    if (current_direction.x != 0)
                    {
                        if (current_direction.x == 1)
                        {
                            if (body[0].getPosition().y == previous_pos[1].y - 20)
                            {
                                corner_interpolation_tail_2.setRotation(0);
                            }
                            else
                            {
                                corner_interpolation_tail_2.setRotation(270);
                            }
                        }
                        else
                        {
                            if (body[0].getPosition().y == previous_pos[1].y - 20)
                            {
                                corner_interpolation_tail_2.setRotation(90);
                            }
                            else
                            {
                                corner_interpolation_tail_2.setRotation(180);
                            }
                        }
                    }
                    else
                    {
                        if (current_direction.y == 1)
                        {
                            if (body[0].getPosition().x == previous_pos[1].x - 20)
                            {
                                corner_interpolation_tail_2.setRotation(0);
                            }
                            else
                            {
                                corner_interpolation_tail_2.setRotation(90);
                            }
                        }
                        else
                        {
                            if (body[0].getPosition().x == previous_pos[1].x - 20)
                            {
                                corner_interpolation_tail_2.setRotation(270);
                            }
                            else
                            {
                                corner_interpolation_tail_2.setRotation(180);
                            }
                        }
                    }

                    // tail_angle_2
                    if (current_direction.x != 0)
                    {
                        if (body[0].getPosition().y == previous_pos[1].y + 20)
                        {
                            tail_angle_2 = 180;
                        }
                        else
                        {
                            tail_angle_2 = 0;
                        }
                    }
                    else
                    {
                        if (body[0].getPosition().x == previous_pos[1].x + 20)
                        {
                            tail_angle_2 = 270;
                        }
                        else
                        {
                            tail_angle_2 = 90;
                        }
                    }
                }
                else
                    corner_interpolation_tail_2.setPosition(0.f, 0.f);
            }

            // main body
            for (int i = 1; i < snake_size - 1; i++)
            {
                if (body[i - 1].getPosition().x == body[i + 1].getPosition().x || body[i - 1].getPosition().y == body[i + 1].getPosition().y)
                {
                    // line
                    body[i].setRotation((body[i - 1].getPosition().x != body[i + 1].getPosition().x) * 90);

                    if (i == snake_size - 2)
                        corner_interpolation_tail_1.setPosition(0.f, 0.f);
                }
                else
                {
                    // corner
                    // find the position of body[i - 1]
                    if (body[i - 1].getPosition().x == body[i].getPosition().x)
                    {
                        // body[i - 1] is either up or down
                        if (body[i - 1].getPosition().y == body[i].getPosition().y + 20)
                        {
                            // body[i - 1] is down
                            // check for body[i + 1]
                            if (body[i + 1].getPosition().x == body[i].getPosition().x + 20)
                            {
                                // corner top-left
                                body[i].setRotation(0);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                            else
                            {
                                // corner top-right
                                body[i].setRotation(90);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                        }
                        else
                        {
                            // body[i - 1] is up
                            // check for body[i + 1]
                            if (body[i + 1].getPosition().x == body[i].getPosition().x - 20)
                            {
                                // corner bottom-right
                                body[i].setRotation(180);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                            else
                            {
                                // corner bottom-left
                                body[i].setRotation(270);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                        }

                        body[i].setRotation(90);
                    }
                    else
                    {
                        // body[i + 1] is either up or down
                        if (body[i + 1].getPosition().y == body[i].getPosition().y + 20)
                        {
                            // body[i + 1] is down
                            // check for body[i - 1]
                            if (body[i - 1].getPosition().x == body[i].getPosition().x + 20)
                            {
                                // corner top-left
                                body[i].setRotation(0);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                            else
                            {
                                // corner top-right
                                body[i].setRotation(90);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }

                            body[i].setRotation(90);
                        }
                        else
                        {
                            // body[i + 1] is up
                            // check for body[i - 1]
                            if (body[i - 1].getPosition().x == body[i].getPosition().x - 20)
                            {
                                // corner bottom-right
                                body[i].setRotation(180);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                            else
                            {
                                // corner bottom-left
                                body[i].setRotation(270);

                                corner_interpolation.emplace_back(body[i]);

                                if (i == snake_size - 2)
                                {
                                    corner_interpolation_tail_1 = body[i];
                                    tail_angle_1 = body[snake_size - 1].getRotation();
                                }
                            }
                        }

                        body[i].setRotation(0);
                    }

                    corner_interpolation[corner_interpolation_count++].setTexture(&texture_snake_corner);

                    if (i == snake_size - 2)
                        corner_interpolation_tail_1.setTexture(&texture_snake_corner);
                }
            }

            accumulator -= dt;
            total_time += dt;
        }

        double alpha = accumulator / dt;

        // drawing
        window.clear(sf::Color(10, 10, 10));
        window.setView(view);

        window.draw(food);

        // tail
        sf::RectangleShape temp_draw = body[snake_size - 1];

        if (corner_interpolation_tail_2.getPosition().x != 0.0f)
        {
            //std::cout << tail_angle_2 << std::endl;
            temp_draw.setRotation(tail_angle_2);
        }

        temp_draw.setPosition(body[snake_size - 1].getPosition().x * alpha + (1.f - alpha) * previous_pos[snake_size - 1].x, body[snake_size - 1].getPosition().y * alpha + (1.f - alpha) * previous_pos[snake_size - 1].y);
        window.draw(temp_draw);

        // main body //
        for (int i = 1; i < snake_size - 1; i++)
        {
            temp_draw = body[i];
            temp_draw.setTexture(&texture_snake_line);

            temp_draw.setPosition(body[i].getPosition().x * alpha + (1.f - alpha) * previous_pos[i].x, body[i].getPosition().y * alpha + (1.f - alpha) * previous_pos[i].y);
            window.draw(temp_draw);
        }

        // head
        temp_draw = body[0];
        temp_draw.setPosition(body[0].getPosition().x * alpha + (1.f - alpha) * previous_pos[0].x, body[0].getPosition().y * alpha + (1.f - alpha) * previous_pos[0].y);
        window.draw(temp_draw);

        // corners
        for (int i = 0; i < corner_interpolation_count; i++)
        {
            window.draw(corner_interpolation[i]);
        }

        // corner_tail
        if (corner_interpolation_tail_2.getPosition().x != 0.f)
        {
            //corner_interpolation_tail_2.setPosition(0.f, 0.f);
            window.draw(corner_interpolation_tail_2);
        }

        window.display();
    }

    return 0;
}
