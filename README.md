# 2D-Shooter-MachineLearning

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
And the second task being the combat aspect.

<br>

**More about reinforcement learning and genetic algorithms**
---
######Reinforcement Learning
RL is a machine learning technique where an agent learns trough experience. The agents preforms an action within an environment, which is interpreted into a reward and a representation of the state, which are then criticized trough negative and positive rewards based on a reward function from within the program, to tell the agent how good its action was and how it should should adjust accordingly. 
Today there are several RL algorithms including TD, Q-learning, and Saras. My AI is trained using Q-learning, The main purpose of all the algorithms is to find an optimal policy. The policy is a mapping between states and actions, it provides the path the agents should follow to maximize the reward function. This is simillar to processes that occur in nature; For example our brains are hard wired to interpret signals such as pain and hunger as negative renforcements, and pleasure and food as positive onces, and this is exactly how our RL agent works. When our agent eats it will be rewarded for taking that action, but when our agent suffers from hunger it will die and will be punished for its most resent actions which will include not eating.

######Genetic Algorithms

<br>

[Back to top](#readme)
