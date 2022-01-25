<br>

# Reinforcement-Learning-in-a-2D-Shooter

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
RL is a machine learning technique where an agent learns trough experience. The agents preforms an action within <br>
an environment, which is interpreted into a reward and a representation of the state, which are then criticized <br>
trough negative and positive rewards based on a reward function from within the program, to tell the agent <br>
how good its action was and how it should should adjust accordingly. <br>
Today there are several RL algorithms including TD, Q-learning, and Saras. My AI is trained using Q-learning. <br>
The main purpose of all the algorithms is to find an optimal policy. The policy is a mapping between states and actions,<br>
it provides the path the agents should follow to maximize the reward function. This is simillar to processes that occur in nature; <br>
For example our brains are hard wired to interpret signals such as pain and hunger as negative renforcements, <br>
and pleasure and food as positive onces, and this is exactly how our RL agent works. When our agent eats <br>
it will be rewarded for taking that action, but when our agent suffers from hunger, <br>
it will die and will be punished for its most resent actions which will include not eating.<br>

---

### Genetic Algorithms
GA is a type of optimization algorithm based on natural selection, Genetic algorithms is one branch of the larger class evolutionary algorithms (EA). They imitate the biological processes of reproduction and natural selection to solve for the ‘fittest’ solutions. Like in evolution, many of the processes involve randomness, however GA allows one to set the level of randomization and the level of control. They raquire no extra information about the given problem, this allows them to find solutions to problems that other optimization methods cannot<br>
handle due to lack of continuity, linearity, or other.<br>
GA work by starting from an initial generation that is then evaluated trough a user defined function. From there subsequent generations<br>
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
algorithms were first being developed. It is also known as roulette wheel selection due to  <br>
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
Stochastic Selection is based upon the fitness proportional selection type; <br>
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
stochastic Selection as this one allowed enough variation while disallowing the fittest agents to saturate <br>
from one generation to another. <br>
	
<br><br>
	
> #### *Crossover*
Crossover is a genetic operator used to give variation to the agents from one generation to the next. <br>
Crossover is sexual reproduction. Two agents are picked from the mating pool at random <br>
to crossover in order to produce superior offspring.<br>
<br>
Also in crossover there are multiple methods you can apply to your algortihm<br>
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
In mutation we take a parent and two variables, the mutation rate; that determs how much of the agent we will mutate and <br>
the muation-amplitude; that determens how much we will change that variable.<br>
Mutation allows GA to avoid falling in to local minima and helps them explore the solution space.<br>
Mutation is done bit by bit, but the mutation rate can change. It can go from 0% to 100%. However, the mutation is typically<br>
around 1%. Anything much higher, will introduce too much randomness, anything less, and you<br>
don’t get enough and the sample tends to stagnate. I went with a mutation rate of 1% and amplitude of 3%.<br>
However do keep in mind the number is also depended on your q-factors if you have high renforcement number then its should be higher<br>
if thy're low it should be lower.

<br>

**Method**
---
As mentiond previously I decide to split up the task in to 2 tasks; A navigation task and a Combat task.<br>
The layout of the environment for both tasks stays the same but the equipment inside changes depending on the task,<br>
The RL algorithm that I used is simplified version Q-Learning as it does not require a model of the environment, and it can handle problems with<br>
transitions and rewards without requiring adaptations. Our agent has 2*16 input nodes each one corresponding to one ray,<br>
16 rays for looking for food or enemies, and 16 for sensing the surrounding environment with <br>
a viewangle between -45 and +45 degrees and a viewrange of 50 pixels.<br>
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
The RL bot is equipped with 3 times 8 sensors, 8 for item seeking, 8 looking at its surroundings and another 8 for accelerating and deccelerating, <br>
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
	
### Result
	
<img src="./myScreen.gif" alt="Gen 1 Using SUS" width="500" height="600">
	
<br>
	
**Combat Task**
---
	
The aim of the combat task was to see how well the agent could learn to aim and shoot against a wandering enemy>
For the combat task 2 input nodes were added, this allows the bot to decide whether or not to shoot,
whether or not the bot shoots is depended on the output of the other sensors and wheather the enemy is in sight.



	
<br>

[Back to top](#readme)
