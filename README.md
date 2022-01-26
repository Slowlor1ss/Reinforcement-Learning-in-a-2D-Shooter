<br>

# Reinforcement-Learning-in-a-2D-Shooter

[Introduction](#Introduction)<br>
[about reinforcement learning and genetic algorithms](#More-about-reinforcement-learning-and-genetic-algorithms)<br>
[Method](#Method)<br>
[Result](#Result)<br>
[Conclusion](#Conclusion)<br>

<br>

**Introduction**
---
While it has been done before, there has been less research in the application of reinforcement learning (RL) to <br>
modern computer games, besides from games like Doom, Minecraft, StarCraft, Tetris, and car racing games.<br>
More often research using RL goes to robotics and multi-agent systems (MAS).<br>
Shooter games have some common features to these fields such as agents having the ability to sense and act to their<br>
surroundings, and complex continuous movements.<br>
Therefore, I found the affects of RL in an 2D Shooter enviroment an interesting area to research.<br>
I will attempt to combine both RL and genetic algorithms (GA) and apply them to my 2D shooter game.<br>
A bot artificial intelligence (AI) generally consists of pathfinding, picking up and using items within<br>
the enviroment and various styles of combat.<br>
Bot AI in commercial games generally uses rule-based systems like, state machines, behaviour trees, etc. These<br>
techniques are typically time-consuming and require fine-tuning of parameters. RL is an interesting algorithm to compete with these systems.<br>
RL could be applied with different parameters and used to generate different bots.<br>
<br>
The aim of this reasearch paper is to see how well I can use RL to learn a bot basic AI behaviours.<br>
To do this I will split the task up in to two major tasks; one being navigation, exploring the environment and item collection.<br>
And the second task being the combat aspect.<br>

<br>

**More about reinforcement learning and genetic algorithms**
---
### Reinforcement Learning
RL is a machine learning technique where an agent learns through experience. The agents perform an action within an environment. <br>
This is then interpreted into a reward and a representation of the state, <br>
which is then criticized through negative and positive reward - based on a reward system within the program - <br>
in order to tell the agent how good its action was and how it should adjust accordingly <br>
Today there are several RL algorithms including TD, Q-learning, and Saras. My AI is trained using Q-learning. <br>
The main purpose of all the algorithms is to find an optimal policy. The policy is a mapping between states and actions,<br>
it provides the path the agents should follow to maximize the reward function. This is similar to processes that occur in nature. <br>
For example, our brains are hard-wired to interpret signals such as pain and hunger as negative reinforcements, <br>
and pleasure and food as positive ones, and this is exactly how our RL agent works. <br>
When our agent eats, it will be rewarded for taking that action, but when our agent <br>
suffers from hunger, it will die and be punished for its most <br>
recent actions - which will include not eating.<br>

---

### Genetic Algorithms
GA is a type of optimization algorithm based on natural selection. Genetic algorithms is one branch <br>
of the larger class evolutionary algorithms (EA). <br>
They imitate the biological processes of reproduction and natural selection to solve for the ‘fittest’ solutions. <br>
Like in evolution, many of the processes involve randomness, however <br>
GA allows one to set the level of randomization and the level of control. <br>
TThey require no added information about the given issue. <br>
This allows them to find solutions to problems that other optimization methods cannot handle due to <br>
the lack of continuity, linearity or ect...<br>
GA works by starting from an initial generation that is then evaluated through a used-defined function. <br>
From there subsequent generations<br>
will be generated trough [***selection***](#selection), [***crossover***](#crossover), and [***mutation***](#mutation).<br>

<br>

> #### *Selection*
Each part of the genetic algorithm has several different ways that it can be executed.<br>
Selection will retain the best performing agents from one generation to the next.<br>
Herefore I tried the 3 following techniques.<br>

<details open><summary><i> Fitness proportionate selection</i> (Click to open/close)</summary>

<br>

```c++
//Natural Selection
//Code that selects a parent to inherent the brain from,
//agents with higher fitness will have more chance to pass on their brain than agents with lower fitness
//Fitness proportionate selection
Elite::FMatrix* Population::SelectParentFPS(const float sum) const
{
	const float rand{ randomFloat(sum) };

	float runningSum{};

	for (size_t i{0}; i < m_Bots.size(); ++i)
	{
		runningSum += m_Bots[i]->GetFitness();

		if (runningSum >= rand)
			return m_Bots[i]->GetBotBrain();
	}
	return nullptr;
}
```
</details>
Fitness proportionate is the first type of selection that was introduced when genetic <br>
algorithms were first being developed. It is also known as 'roulette wheel selection' due to  <br>
the similarity of selection it has with a physical roulette wheel.  <br>
How it works is that for each element in the set it finds the sum  <br>
from F<sub>0</sub> . . . F<sub>n</sub> and gives each element a chance  <br>
to be selected with the individual fitness over the sum of the fitness.  <br>
	
<br>

<details open><summary><i>Stochastic universal sampling</i></summary>

<br>
	
```c++
//Stochastic universal sampling
void Population::SelectParentSUS(const float sum) const
{
	std::vector<FMatrix*> matingPool{};
	std::vector<float> pointers{};

	const auto maxFit{ sum };
	constexpr auto num{ 25 }; //Number of offspring to keep
	const auto dist = maxFit / static_cast<float>(num);
	const auto start = randomFloat(0, dist);
	for (size_t i{ 0 }; i < num; ++i)
	{
		pointers.push_back(start + static_cast<float>(i) * dist);
	}

	for (size_t i{ 0 }; i < pointers.size(); ++i)
	{
		auto j = 0;
		while (CalculateFitnessSum(0, j) < pointers[i])
		{
			j++;
		}
		matingPool.push_back(m_Bots[j]->GetBotBrain());
	}

	for (size_t i{ 0 }; i < m_Bots.size(); ++i)
		m_Bots[i]->SetBotBrain(*matingPool[i % matingPool.size()]);
}
```
</details>
Stochastic Selection is based upon the fitness proportional selection type.<br>
However, it is made to be fairer. It uses a single random value to get<br>
a sampling of all the agents by choosing them at evenly spaced intervals.<br>

<br>

<details open><summary><i>Tournament selection</i></summary>

<br>

```c++
//Tournament selection
QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
```
</details>
Tournament selection is one of the simpler<br>
methods of selection. This type of selection simply works by determining which <br>
of the agents has the best fitness, and chooses that one.<br>

<br>

After implementing and testing each of the techniques, I decided to go with<br>
stochastic Selection as this one allowed enough variation while preventing the fittest agents to saturate <br>
from one generation to another. <br>
	
<br><br>
	
> #### *Crossover*
Crossover is a genetic operator used to give variation to the agents from one generation to the next. <br>
Crossover is sexual reproduction. Two agents are picked from the mating pool at random <br>
to crossover in order to produce superior offspring.<br>
<br>
Also, in crossover there are multiple methods you can apply to your algorithm.<br>
Some of the different types include:<br>
<br>
**Single Point Crossover**: Where one point on the parent organism is selected. <br>
All data beyond that point in the organism is swapped between the two parent organisms.<br>
	
<p>
<img src="https://media.geeksforgeeks.org/wp-content/uploads/20190620121215/singleCrossover-2.png" width=50% height=50%>
</p>
	
<br>

**Two-Point Crossover**: This is a specific case of a N-point Crossover technique. <br>
Two random points are chosen on the individual and the genetic material is exchanged at these points.<br>
	
<p>
<img src="https://media.geeksforgeeks.org/wp-content/uploads/20190620121313/twopointCrossover-2.png" width=50% height=50%>
</p>
	
<br>
	
**Uniform Crossover**: Like tossing a coin.<br>
Each bit is selected randomly from one of the corresponding genes of the parent chromosomes.<br>
	
<p>
<img src="https://media.geeksforgeeks.org/wp-content/uploads/20190620121403/unifromCrossover-2.png" width=50% height=50%>
</p>

<br><br>	

> #### *Mutation*
In mutation we take a parent and two variables, the mutation rate; that determines how much of the agent we will mutate and <br>
the mutaion-amplitude; that determines how much we will change that variable.<br>
Mutation allows GA to avoid falling in to local minima and helps them explore the solution space.<br>
Mutation is done bit by bit, but the mutation rate can change. It can go from 0% to 100%. However, the mutation is typically<br>
around 1%. Anything much higher, will introduce too much randomness, anything less, and you<br>
don’t get enough and the sample tends to stagnate. I went with a mutation rate of 1% and amplitude of 3%.<br>
However, do keep in mind the number is also depended on your q-factors. If you have  high reinforcement numbers then it should be higher;<br>
if they're lower, it should be lower.<br>

<br>

**Method**
---
As mentioned previously, I decided to split up the tasks into two; a navigation tast and a combat task.<br>
The layout of the environment for both tasks changes depending on the task.<br>
The RL algorithm that I used is a simplified version Q-Learning as it does not require a model of the environment, and it can handle problems with<br>
transitions and rewards without requiring adaptations. Our agent has 3 times 8 input nodes each one corresponding to one ray.<br>
8 Rays for looking for food or enemies, 8 for sensing the surrounding environment with, and 8 for accelerating and deaccelerating <br>
A view-angle between -45 and +45 degrees and a viewrange of 50 pixels.<br>
In total these return 11 output nodes returning an rotation value between -pi and +pi.<br>

<br>
For the GA, (as previously explained) I decided to use stochastic Selection <br>
as this one allowed enough variation while disallowing the fittest agents to saturate <br>
from one generation to another. Uniform crossover, and a mutation rate of 1% and amplitude of 3%.<br>

<br>	

**Navigation Task**
---
	
The aim of the navigation task is to train the bot to learn to traverse and explore a maze-type environment<br>
while also picking up food items.<br>
	
### Setup

The test environment is a square of 140 x 140 pixels. The map is surrounded by walls so that the agent is locked inside of the map,<br>
within there's a total of 50 food spawnpoints and 8 obstacles within the map, the agent cannot move or look trough these walls.<br>
Figure 1 shows an image of the map.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/Map.png?raw=true" width=25% height=25%><br>
<b>Figure 1. Map. Blue dots represent food objects, green squares represent obstacles.</b>
</p>

<br>
The RL bot is equipped with 3 times 8 sensors, 8 for item seeking, 8 looking at its surroundings and another 8 for accelerating and deaccelerating, <br>
The sensors range from 45 degrees to the left to 45 degrees to the right.<br>
The first 8 sensors were used to determine if the was food in the view of the bot.<br>
Then the distance to that food object is calculated and inverted, the inverted distance is saved<br>
in the memory matrix of our RL agent. <br>
We save the inverted distance cause we only have one memory matrix and want the closest food to have the most impact<br>
this means that if our food is at the same position we are (distance = 0) our inverted distance = max view range,<br>
because of how matrix multiplication work this makes it easier while working with one memory matrix.<br>
Our second 16 sensors are used to determine if there are any obstacles in the agents vision,<br>
also for these objects we calculate the distance and then check how far this obstacle is,<br>
When the obstacle is far away from out agent we reward it with with a small reward of 0.0001.<br>
If an obstacle was close to our agent we give it a penalty of -0.1, and when theres no<br>
obstacle in our view then we reward it with 0.001.<br>
Our agent can accordingly to these parameters choose a direction between -pi and +pi.<br>
Figure 2 shows a agent with 16 rays, the blue dots represent food and the blue dots with green circles<br>
represent food within our view, and the green ray represnts the newly chosen direction.<br>
<br>
<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/viewRange.png?raw=true" width=40% height=40%><br>
<b>Figure 2. Agent with view rays.</b>
</p>
	
<br>
	
**Combat Task**
---
	
The aim of the combat task was to see how well the agent could learn to aim and shoot against a wandering enemy.<br>
For the combat task 2 input nodes were added, this allows the bot to decide whether or not to shoot,<br>
whether or not the bot shoots is depended on the output of the other sensors and wheather the enemy is in sight.<br>

<br>
	
**Result**
---

I started off testing and experementing with an environment that only has food, and testing the different selection methods on it.<br>
	
<br>
	
### Testing the different selection methods using a simple problem

<br>
	
> #### *Generation 1 Using Stochastic universal sampling*
<br>
We can see that in the first generation most agents are clueless of what they're doing, <br>
but 2 agents seem to figure out that they have move over the food to survive<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/SUSFood.gif?raw=true" alt="Gen 1 Using SUS" width="50%" height="50%"><br>
</p>

<br>
	
> #### *Generation 10 Using Stochastic universal sampling*
	
After about 10 generations we can see that most agents seem to have figured out that they need to eat the food,<br>
there was also a large improvement in their ability to survive as long as possible, <br>
but we can also see that some agents havent quite figured it out yet. <br>
For a simple task like this, tournament selection could be a better choice.<br>
	
<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/SUSFoodGen10.gif?raw=true" alt="Gen 1 Using SUS" width="50%" height="50%"><br>
</p>
	
<br>
	
> #### *Generation 3 Using tournament selection*

We can see that using tournament selection as good as all agents have a general idea of what to do, <br>
they are also able to outprform the agents using Stochastic universal sampling,<br>
but we can noticably see a decrease in variation of the agents, which is one of the reasons its not often used for more complex problems.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/TSFoodGen3.gif?raw=true" alt="Gen 3 Using TS" width="50%" height="50%"><br>
</p>
	
<br>
	
> #### *Generation 10 Using fitness proportionate selection*

And lastly using fitness proportionate selection, we can see that this is a situation where this type of selects lacks compentence<br>
due to it being too random, as member of the population has a really large fitness in comparison with other members.<br>
And thus the chances are that this one will not be included in the next generation, preventing it form evolving as quickly as the other methods.<br>
Note however that this isn't always the case due to it being random it can slo have good performance but <br>
whether or not it will be good in a situation like this is random.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/FPSFoodGen10.gif?raw=true" alt="Gen 10 Using FPS" width="50%" height="50%"><br>
</p>

### Navigation Task Results

<br>
	
> #### *Generation 1*

In the first few generation we can see that our agents have no clue at all of what they're doing and lots of them are just plainly running in to walls,<br>
in some of the first iterations they even managed to clip trough walls, however all these agents die as theres nothing behind the walls.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/NavGen1.2.gif?raw=true" width="50%" height="50%"><br>
</p>

<br>
	
<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/NavGen1Escapies.gif?raw=true" width="50%" height="50%"><br>
<b>An example of a generation where they managed to clip trough walls</b>
</p>

<br>
	
> #### *Generation 10*

We can see that after 10 generations most of our agents are still bumping in to lots of walls, <br>
but we can also see that some agents are starting to get a bit of a sense of their environment<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/NavGen10.gif?raw=true" width="50%" height="50%"><br>
</p>

<br>
	
> #### *Generation 15*

5 generations later we see some improvement, they are just circeling around anymore but also start to explore different areas of the map, <br>
but this still isn't very good looking.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/NavGen15Short.gif?raw=true" width="50%" height="50%"><br>
</p>
	
<br>
	
> #### *Generation 40*
	
After 40 generation we can see the agents learnt a new strategy, that includes moving slower in order to hit less walls.<br>
We can see that some of the agents have come up with a wandering style of exploring the environment while still taking in to account where the walls are.<br>
Still this isn't the best, looking back at it it would have probably been better to start with an easier to navigate environment or have a clearer goal.<br>
	
<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/NavGen40.gif?raw=true" width="50%" height="50%"><br>
</p>
	
<br>
	
### Combat Task Results

<br>
	
<p>
For the combat task I placed all agents in an open area where they are free to move how they like and shoot whenever they want, <br>
the enemy is trimmed to the size of the map and going out of the map on the once side will result on coming back in on the other side.<br>
Hitting and enemy will be largely positivly renforced as this does not happen often and missing will be slightly negatively renforced as this happens often.<br>
Do note that even tho it seems as they can shoot infintly far only hits when the enemy was in the view range are counted as actual hits, <br>
this is to prvent renforcing hit that happen by sheer luck.<br>

In early generations we can see that some agents are starting to undestand that they should follow the enemy <br>
and aim at it but they still miss most of their shots.<br>
</p>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/ShootingMessy.gif?raw=true" width="50%" height="50%"><br>
</p>
	
As training 50 agents at once shooting is a bit messy to look at i have also taken the best agent out of one of the generations.<br>
Here we can see that the agent already learned to shoot at the enemy, only it does not know when to stop shooting <br>
and when there no enemy nearby it will also start shooting at random.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/ShootingOneAgentEarlyGen.gif?raw=true" width="50%" height="50%"><br>
</p>
	
After lots of generations we can see that they stop shooting at random all the time and start to shoot more selectively,<br>
we can also see that theres clearly being aimed at the enemy and it gets hit often.<br>

<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/ShootingAfterManyGens.gif?raw=true" width="50%" height="50%"><br>
</p>

### Combined Task Results

<br>
When combining both tasks we can see that the agents will wander untill they see the enemy, once they spot him they will shoot and approach him.<br>
While still a large part of the agents miss shots and those that can't find the enemy get a bit clueless at times, <br>
those that do manage to get the enemy in their view will hit a lare part of thier shots.<br>
<br>
<p align="center">
<img src="https://github.com/Slowlor1ss/2D-Shooter-MachineLearning/blob/main/source/Media/Combined.gif?raw=true" width="50%" height="50%"><br>
</p>
	
<br>
	
***Conclusion***
---
	
<p>
<br>
The aim of this paper was to see how well I can use RL to learn a bot basic AI behaviours and how well it would preform in a 2d shooter,<br>
and from my attempt I can conslude that basic AI behaviours aren't too hard to teach, but learning an agent how to properly navigate or explore<br>
a map is a lot harder, I do believe that it is possible, if you have the rquired knowledge and time for it, <br>
and when you do manage to do it properly i can see various advantages for using RL over rule-based systems such as;<br>
minimal code needed for the underlying algorithm in some cases, decrease in the time spent tuning parameters for different situations, <br>
and maybe the most promising, the ability to produce different bot personality types.<br>
I'd say that teaching an agent how to properly explore a room was a bit too hard for <br>
my low expirence when starting this project combined with the limited time, <br>
it might have been better to start of simpler. However I did learn a lot about machine learning in general <br>
and I have improved a lot, so therefore when I have time I might restart or contiue on this project and try to make something better.<br>
Further work could be spent on trying different environmental setups with the same agents to see how well they preform in an unknown enviroment.<br>
Some of the task have already been tested in different environmental setups like the searching for items and shooting an enemy as those setups where random.<br>

Whatever the future holds for RL in games I can't tell, <br>
but I can confidently say that I see potential in it, and when you combine it with more convencial rule-based systems I believe you<br>
can get better results than by only using these rule-based systems.<br>
</p>

<br>

[Back to top](#readme)
