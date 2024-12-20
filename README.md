# Chinese Word Segmentation

## 整体思路：

​	读入字典文件，在内存中建立一个链表，存入所有词、词频、词性以便后续分词使用。首先考虑最简单的情况，即句子中不含有任何中文标点符号，因为此时句子不会很长，所以用邻接链表存储整个句子并不会耗费太多的空间。节点数是句子长度加一，句子中的每个汉字都是一条边，边的权值和词频呈倒数关系。由于句子的邻接链表是有向无环图，所以可以采用压缩存储方式节省一半的空间。然后利用迪杰斯特拉算法，找到最短路径，并输出分词结果。

## 细节展示：

### 准备工作：

```c++
typedef struct Node {
	string word;
	string property;
	int frequency;
	struct Node* next;
}Node;
```

​	对于字典中的每一个词，该结构体Node都包含词、词频、词性、指向下一个词的指针。

```c++
Node* Begin = new Node;
Node* End = new Node;
// word = double byte!!!
int maxlength = 0;
queue<string> q;
stack<string> s;
const int N = 105, MAX = ~(1<<31);
double arcs[N][N];
double dist[N];
int path[N];
int vertex[N];
bool state[N];
int cnt = 0;
int punctuation[N];
```

​	创建了该字典的头节点Begin和尾节点End，maxlength代表的是字典中词的最大长度，队列和栈在前向最大匹配法、后向最大匹配法以及双向最大匹配法中有所应用，数组的创建在迪杰斯特拉算法中有所应用。arcs代表以词为元素的边构成的词频矩阵，dist代表从初始点到其他结束点的最短距离，path代表从初始点到结束点的最短路径，vertex代表最短路径经过的各个点，state代表该点是否经过，punctuation在中文标点符号的转换方面有所应用。

### 读取字典并建立链表：

```c++
string UTF8ToGBK(const char* strUTF8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len+1];
    memset(wszGBK, 0, len*2+2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len+1];
    memset(szGBK, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if(wszGBK) delete[] wszGBK;
    if(szGBK) delete[] szGBK;
    return strTemp;
}
```

​	UTF8ToGBK函数的主要功能是将UTF8编码的字符转换为GBK编码的字符，使后续操作更方便。

```c++
Status LoadDictionary(string filename, string &content) {
	// Load the file
	ifstream file(filename);
	// Fail to open the file
	if (!file.is_open()) {
		cout<<"ERROR: Cannot open the file!"<<endl;
		return ERROR;
	}
	string line;
	while (getline(file, line)) content += line + '\n';
	file.close();
	// Convert UTF-8 to GBK
	const char* constc = nullptr;
	constc = content.c_str();
	content = UTF8ToGBK(constc);
	cout<<"The dictionary has been successfully loaded!"<<endl;
	return OK;
}
```

​	LoadDictionary函数主要功能是将字典的文本文件读入并存入content中，如果查询不到文件名称或者无法打开文件，会报出错误信息。其中使用了字符编码转换函数UTF8ToGBK。

```c++
void CreateDictionary(string content) {
	string temp = "";
	Node* p = Begin;
	for (int i = 0; i < content.length(); ++i) {
		if (content[i] == '\n' and temp != "") {
			// Extract word, frequency and property
			int j = 0, word_position = 0, frequency_position = 0, length = temp.length();
			while (j < length) {
				if (temp[j] == ' ')
					if (!word_position) word_position = j;
					else frequency_position = j;
				++j;
			}
			// Calculate the length of the longest word
			if (word_position > maxlength) maxlength = word_position;
			// Create the Node and convert frequency to int
			Node* block = new Node;
			block->word = temp.substr(0, word_position);
			block->property = temp.substr(frequency_position+1, length-frequency_position-1);
			stringstream ss;
			string strnum = temp.substr(word_position+1, frequency_position-word_position-1);
			int num;
			ss << strnum;
			ss >> num;
			block->frequency = num;
			// Link the Node
			p->next = block;
			p = block;
			// Initialization of temp
			temp = "";
		}
		else temp += content[i];
	}
	p->next = End;
	End->next = nullptr;
	cout<<"The dictionary has been successfully created!"<<endl;
	return;
}
```

​	CreateDictionary函数的主要功能是将content中的词语分开并存入到链表中，以便后续查询使用。

### 前向最大匹配法：

```c++
void Forward_Maximum_Matching_Method(string sentence) {
	// Matching word from the beginning of the sentence
	string temp;
	while (sentence != "") {
		int sentence_length = sentence.length();
		if (sentence_length > maxlength)
			temp = sentence.substr(0, maxlength);
		else
			temp = sentence;
		
		int flag = 0;
		while (!flag) {
			int temp_length = temp.length();
			// Single character
			if (temp_length == 2) {
				q.push(temp);
				sentence = sentence.substr(2, sentence_length);
				break;
			}
			Node* p = Begin->next;
			while (p->next) {
				if (temp == p->word) {
					q.push(temp);
					sentence = sentence.substr(temp_length, sentence_length-temp_length);
					flag = 1;
					break;
				}
				p = p->next;
			}
			if (!p->next) temp = temp.substr(0, temp_length-2);
		}
	}
	return;
}
```

​	Forward_Maximum_Matching_Method函数的主要功能是利用前向最大匹配法分词，具体思路如下。首先找到字典中词语的最大长度，然后对于该句子前面maxlength的词语，寻找是否在字典中。如果不在，那么长度减一，继续从前向后寻找，直至找到在字典中的词语或者剩下长度为一的词语为止。如果在，那么将该长度的词语入队列，在句子中删掉该词语，继续从最大长度开始寻找，重复上述步骤，直至句子为空。

```c++
void Output_Forward() {
	string temp = "";
	while (!q.empty()) {
		temp += q.front();
		if (q.size() != 1) temp += '/';
		else temp += '\n';
		q.pop();
	}
	cout<<temp;
	return;
}
```

​	Output_Forward函数的主要功能是输出前向最大匹配法的结果，同时清空队列。

### 后向最大匹配法：

```c++
void Backward_Maximum_Matching_Method(string sentence) {
	// Matching word from the ending of the sentence
	string temp = "";
	while (sentence != "") {
		int sentence_length = sentence.length();
		if (sentence_length > maxlength)
			temp = sentence.substr(sentence_length-maxlength, maxlength);
		else
			temp = sentence;
		
		int flag = 0;
		while (!flag) {
			int temp_length = temp.length();
			// Single character
			if (temp_length == 2) {
				s.push(temp);
				sentence = sentence.substr(0, sentence_length-2);
				break;
			}
			Node* p = Begin->next;
			while (p->next) {
				if (temp == p->word) {
					s.push(temp);
					sentence = sentence.substr(0, sentence_length-temp_length);
					flag = 1;
					break;
				}
				p = p->next;
			}
			if (!p->next) temp = temp.substr(2, temp_length);
		}
	}
	return;
}
```

​	Backward_Maximum_Matching_Method函数的主要功能是利用后向最大匹配法分词，具体思路如下。首先找到字典中词语的最大长度，然后对于该句子后面maxlength的词语，寻找是否在字典中。如果不在，那么长度减一，继续从后向前寻找，直至找到在字典中的词语或者剩下长度为一的词语为止。如果在，那么将该长度的词语进栈，在句子中删掉该词语，继续从最大长度开始寻找，重复上述步骤，直至句子为空。

```c++
void Output_Backward() {
	string temp = "";
	while (!s.empty()) {
		temp += s.top();
		if (s.size() != 1) temp += '/';
		else temp += '\n';
		s.pop();
	}
	cout<<temp;
	return;
}
```

​	Output_Backward函数的主要功能是输出后向最大匹配法的结果，同时清空栈。

### 双向最大匹配法：

```c++
void Bidirectional_Maximum_Matching_Method(string sentence) {
	// Select the most optimal results from forward and backward
	Forward_Maximum_Matching_Method(sentence);
	Backward_Maximum_Matching_Method(sentence);
	return;
}
```

​	Bidirectional_Maximum_Matching_Method函数的主要功能是利用双向最大匹配法分词，具体思路如下。结合前向最大匹配法和后向最大匹配法的分词结果，为了最大限度的消除歧义和保持分词结果的合理性，取公共长度最长的作为最终的分词结果。

```c++
void Output_Bidirectional() {
	string temp = "";
	if (q.empty() or s.empty()) cout<<"empty";
	while (!q.empty() and !s.empty()) {
		string forward = q.front(), backward = s.top();
		q.pop();
		s.pop();
		while ((forward != backward) and (!q.empty() and !s.empty())) {
			forward += q.front();
			backward += s.top();
			q.pop();
			s.pop();
		}
		temp += forward;
		if (!q.empty() and !s.empty()) temp += '/';
	}
	temp += '\n';
	cout<<temp;
	return;
}
```

​	Output_Bidirectional函数的主要功能是输出双向最大匹配法的结果，同时清空队列和栈。

### 迪杰斯特拉算法：

```c++
void Adjacent_Matrix(string sentence) {
	// Every character is the edge connected two convexes
	int length = sentence.length()/2+1;
	for (int i = 0; i < length; ++i)
		for (int j = 0; j < length; ++j)
			arcs[i][j] = MAX;
	for (int i = 0; i < length-1; ++i)
		for (int j = i+1; j < length; ++j) {
			string temp = sentence.substr(2*i, 2*(j-i));
			Node* p = Begin->next;
			while (p->next) {
				if (temp == p->word) {
					arcs[i][j] = 10000.0/p->frequency;
					break;
				}
				p = p->next;
			}
		}
	return;
}
```

​	Adjacent_Matrix函数的主要功能是建立邻接矩阵，并对句子所有的子串进行查询。这里邻接矩阵的值一开始选用了词频的倒数，效果不是很好。翻看字典发现词频在几十到几十万不等，于是调整为词频倒数的一万倍，效果明显好了很多。

```c++
void Dijkstra(int length) {
	for (int i = 0; i < length; ++i) {
		dist[i] = arcs[0][i];
		path[i] = (dist[i] < MAX) ? 0 : -1;
		state[i] = false;
	}
	state[0] = true;
	dist[0] = 0;
	for (int i = 1; i < length; ++i) {
		int minimum = MAX, prior = 0;
		for (int j = 0; j < length; ++j) {
			if (!state[j] and (minimum > dist[j])) {
				minimum = dist[j];
				prior = j;
			}
		}
		state[prior] = true;
		for (int j = 0; j < length; ++j) {
			if (!state[j] and (dist[j] > dist[prior]+arcs[prior][j])) {
				dist[j] = dist[prior]+arcs[prior][j];
				path[j] = prior;
			}
		}
	}
	if (path[length-1] == -1) path[length-1] = length-2;
	return;
}
```

​	Dijkstra函数的主要功能是找到从起点到终点的最短路径，采用了贪心算法的思想，具体过程不再赘述。

```c++
void Find(int x) {
	if (path[x] == 0) vertex[cnt++] = 0;
	else Find(path[x]);
	vertex[cnt++] = x;
	return;
}
```

​	Find函数的主要功能是反向将最短路径经过的节点转化为正向路径。

```c++
void Output_Result(string sentence) {
	Adjacent_Matrix(sentence);
	int length = sentence.length()/2+1;
	cnt = 0;
	Dijkstra(length);
	Find(--length);
	for (int i = 0; i < cnt-1; ++i) {
		cout<<sentence.substr(2*vertex[i], 2*(vertex[i+1]-vertex[i]));
		if (i != cnt-2) cout<<'/';
	}
	return;
}
```

​	Output_Result函数的主要功能是将不含标点符号的句子分词并输出。

```c++
void Punctuation_Convert(string sentence, int &num) {
	// Convert the encoding of punctuations
	int length = sentence.length(), l = 14;
	string p[l] = {"，", "。", "、", "？", "！", "：", "；", "“", "”", "‘", "’", "《", "》", "…"};
	for (int i = 0; i < l; ++i) {
		const char* constc = nullptr;
		constc = p[i].c_str();
		p[i] = UTF8ToGBK(constc);
	}
	for (int i = 0; i < length; i += 2) {
		string c = sentence.substr(i, 2);
		for (int j = 0; j < l; ++j)
			if (c == p[j]) punctuation[num++] = i;
	}
	return;
}
```

​	Punctuation_Convert函数的主要功能是将中文标点符号的编码由UTF8转成GBK，以便后续使用。

```c++
void Cut_Sentence(string sentence) {
	int num = 0;
	Punctuation_Convert(sentence, num);
	
	if (!num) Output_Result(sentence);
	else {
		if (punctuation[0]) {
			string temp = sentence.substr(0, punctuation[0]);
			Output_Result(temp);
		}
		for (int i = 0; i < num-1; ++i) {
			if (punctuation[i+1]-punctuation[i] == 2) {
				while (punctuation[i+1]-punctuation[i] == 2) {
					string c = sentence.substr(punctuation[i], 2);
					if (i) cout<<'/';
					cout<<c;
					i++;
				}
				i--;
				continue;
			}
			else {
				string c = sentence.substr(punctuation[i], 2);
				if (punctuation[i]) cout<<'/';
				cout<<c<<'/';
				string temp = sentence.substr(punctuation[i]+2, punctuation[i+1]-punctuation[i]-2);
				Output_Result(temp);
			}
		}
		if (punctuation[num-1] != sentence.length()-2) {
			string c = sentence.substr(punctuation[num-1], 2);
			if (num != 1) cout<<'/';
			cout<<c<<'/';
			string temp = sentence.substr(punctuation[num-1]+2, sentence.length()-punctuation[num-1]-2);
			Output_Result(temp);
		}
		else {
			string c = sentence.substr(punctuation[num-1], 2);
			if (num != 1) cout<<'/';
			cout<<c;
		}
	}
	cout<<endl;
	return;
}
```

​	Cut_Sentence函数的主要功能是处理含有中文标点符号的句子，将标点符号单独输出，剩下的不含标点符号的句子交给Output_Result函数处理。

## 使用说明：

- 本程序为中文分词，只能切分中文文本和中文标点，如果输入带有英文字母或者数字，由于编码长度不同，可能会造成程序崩溃。
- 每次选择字典之后无法更改，想要改变字典只能重新运行。
- 对于每次输入的句子，可以选择四种分词方式。
- 想要退出分词时，只需要输入“exit”即可。

## 测试用例：

原文：在寒冷的冬日里，金色的长裙摇曳，浮光璀璨；深色的西装笔挺，气宇轩昂。同学们整装待发，经过几个月的训练与磨合，他们已经成竹在胸，静待在舞台的华灯之下绽放夺目的光彩。下午，激动人心的登台表演时刻到来，全体参演人员徐徐登场，在聚光灯之下开始了合唱演出。钢琴激昂的音阶倾泻而出，朗诵的声音慷慨激昂，开启了《怒吼吧！黄河》的激昂篇章。随着指挥双臂的挥舞，合唱团的乐音由震撼人心的齐声合唱，再到如泣如诉的声部交替呈现，然后又回到齐声的怒吼之中，逐渐加速的一声声乐句，映出中华儿女对侵略的愤恨，最终，一曲合唱在高昂的战斗的警号中结束，曲尽而意味深长。“怒吼吧，黄河！掀起你的怒涛，发出你的狂叫！向着全世界的人民，发出你的警号！”歌声激昂如磅礴的洪流，汹涌澎湃，激荡人心。“五千年的民族，苦难真不少；铁蹄下的民众，苦痛受不了。”旋律婉转哀伤，道尽中华民族的苦难与悲愤。在悲哀愤恨交杂下，“新中国已经破晓，四万万五千万民众已经团结起来，誓死同把国土保”，发出了中华民族反侵略的最强音。从生疏到默契，从胆怯到自信，从艰难到成功。回望在一起拼搏奋斗的四十多天光阴，我们无比感激有这样的一个音乐盛会让我们相聚于此，互相帮助，互相鼓励，一起克服合唱中的困难，一起迎接站上舞台的喜悦。奖杯是对我们最终作品的肯定，更是对我们联队团结、拼搏之精神，严肃、认真之态度的最高评价。比赛的结束绝不是终点，我们会继续保持热情，继续为团队的荣誉而努力奋斗，铭记这份责任和使命，不辜负时代的机遇和挑战。“好风凭借力，送我上青云”让我们携手努力，风雨同舟，创造更加辉煌的明天！

地址：http://ai.ruc.edu.cn/newslist/newsdetail/20231211005.html

分词结果：

- 前向最大匹配法

```
在/寒冷/的/冬日/里/，/金色/的/长裙/摇曳/，/浮/光/璀璨/；/深色/的/西装/笔挺/，/气宇轩昂/。/同学/们/整装待发/，/经过/几个/月/的/训练/与/磨合/，/他们/已经/成竹在胸/，/静/待/在/舞台/的/华灯/之下/绽放/夺目/的/光彩/。/下午/，/激动人心/的/登台/表演/时刻/到来/，/全体/参演/人员/徐徐/登场/，/在/聚光灯/之下/开始/了/合唱/演出/。/钢琴/激昂/的/音阶/倾泻/而/出/，/朗诵/的/声音/慷慨激昂/，/开启/了/《/怒吼/吧/！/黄河/》/的/激昂/篇章/。/随着/指挥/双臂/的/挥舞/，/合唱团/的/乐音/由/震撼人心/的/齐声/合唱/，/再/到/如泣如诉/的/声部/交替/呈现/，/然后/又/回到/齐声/的/怒吼/之中/，/逐渐/加速/的/一声声/乐句/，/映出/中华/儿女/对/侵略/的/愤恨/，/最终/，/一曲/合唱/在/高昂/的/战斗/的/警/号/中/结束/，/曲/尽/而/意味深长/。/“/怒吼/吧/，/黄河/！/掀起/你/的/怒涛/，/发出/你/的/狂/叫/！/向着/全世界/的/人民/，/发出/你/的/警/号/！/”/歌声/激昂/如/磅礴/的/洪流/，/汹涌澎湃/，/激荡/人心/。/“/五千年/的/民族/，/苦难/真/不少/；/铁蹄/下/的/民众/，/苦痛/受不了/。/”/旋律/婉转/哀伤/，/道/尽/中华民族/的/苦难/与/悲愤/。/在/悲哀/愤恨/交/杂/下/，/“/新/中国/已经/破晓/，/四万万/五千万/民众/已经/团结/起来/，/誓死/同/把/国土/保/”/，/发出/了/中华民族/反/侵略/的/最强音/。/从/生疏/到/默契/，/从/胆怯/到/自信/，/从/艰难/到/成功/。/回望/在/一起/拼搏/奋斗/的/四十多/天光/阴/，/我们/无比/感激/有/这样/的/一个/音乐/盛会/让/我们/相聚/于/此/，/互相/帮助/，/互相/鼓励/，/一起/克服/合唱/中/的/困难/，/一起/迎接/站上/舞台/的/喜悦/。/奖杯/是/对/我们/最终/ 作品/的/肯定/，/更是/对/我们/联队/团结/、/拼搏/之/精神/，/严肃/、/认真/之/态度/的/最高/评价/。/比赛/的/结束/绝不/是/终点/，/我们/会/继续/保持/热情/，/继续/为/团队/的/荣誉/而/努力/奋斗/，/铭记/这份/责任/和/使命/，/不/辜负/时代/的/机遇/和/挑战/。/“/好/风/凭借/力/，/送/我/上/青云/”/让/我们/携手/努力/，/风雨同舟/，/创造/更加/辉煌/的/明天/！
```

- 后向最大匹配法

```
在/寒冷/的/冬日/里/，/金色/的/长裙/摇曳/，/浮/光/璀璨/；/深色/的/西装/笔挺/，/气宇轩昂/。/同学/们/整装待发/，/经过/几个/月/的/训练/与/磨合/，/他们/已经/成竹在胸/，/静/待/在/舞台/的/华灯/之下/绽放/夺/目的/光彩/。/下午/，/激动人心/的/登台/表演/时刻/到来/，/全体/参演/人员/徐徐/登场/，/在/聚光灯/之下/开始/了/合唱/演出/。/钢琴/激昂/的/音阶/倾泻/而/出/，/朗诵/的/声音/慷慨激昂/，/开启/了/《/怒吼/吧/！/黄河/》/的/激昂/篇章/。/随着/指挥/双臂/的/挥舞/，/合唱团/的/乐音/由/震撼人心/的/齐声/合唱/，/再/到/如泣如诉/的/声部/交替/呈现/，/然后/又/回到/齐声/的/怒吼/之中/，/逐渐/加速/的/一声声/乐句/，/映出/中华/儿女/对/侵略/的/愤恨/，/最终/，/一曲/合唱/在/高昂/的/战斗/的/警/号/中/结束/，/曲/尽/而/意味深长/。/“/怒吼/吧/，/黄河/！/掀起/你/的/怒涛/，/发出/你/的/狂/叫/！/向着/全世界/的/人民/，/发出/你/的/警/号/！/”/歌声/激昂/如/磅礴/的/洪流/，/汹涌澎湃/，/激荡/人心/。/“/五千年/的/民族/，/苦难/真/不少/；/铁蹄/下/的/民众/，/苦痛/受不了/。/”/旋律/婉转/哀伤/，/道/尽/中华民族/的/苦难/与/悲愤/。/在/悲哀/愤恨/交/杂/下/，/“/新/中国/已经/破晓/，/四万万/五千万/民众/已经/团结/起来/，/誓死/同/把/国土/保/”/，/发出/了/中华民族/反/侵略/的/最强音/。/从/生疏/到/默契/，/从/胆怯/到/自信/，/从/艰难/到/成功/。/回望/在/一起/拼搏/奋斗/的/四/十多天/光阴/，/我们/无比/感激/有/这样/的/一个/音乐/盛会/让/我们/相聚/于/此/，/互相/帮助/，/互相/鼓励/，/一起/克服/合唱/中/的/困难/，/一起/迎接/站上/舞台/的/喜悦/。/奖杯/是/对/我们/最终/ 作品/的/肯定/，/更是/对/我们/联队/团结/、/拼搏/之/精神/，/严肃/、/认真/之/态度/的/最高/评价/。/比赛/的/结束/绝/不是/终点/，/我们/会/继续/保持/热情/，/继续/为/团队/的/荣誉/而/努力/奋斗/，/铭记/这份/责任/和/使命/，/不/辜负/时代/的/机遇/和/挑战/。/“/好/风/凭/借力/，/送/我/上/青云/”/让/我们/携手/努力/，/风雨同舟/，/创造/更加/辉煌/的/明天/！
```

- 双向最大匹配法

```
在/寒冷/的/冬日/里/，/金色/的/长裙/摇曳/，/浮/光/璀璨/；/深色/的/西装/笔挺/，/气宇轩昂/。/同学/们/整装待发/，/经过/几个/月/的/训练/与/磨合/，/他们/已经/成竹在胸/，/静/待/在/舞台/的/华灯/之下/绽放/夺目的/光彩/。/下午/，/激动人心/的/登台/表演/时刻/到来/，/全体/参演/人员/徐徐/登场/ ，/在/聚光灯/之下/开始/了/合唱/演出/。/钢琴/激昂/的/音阶/倾泻/而/出/，/朗诵/的/声音/慷慨激昂/，/开启/了/《/怒吼/吧/！/黄河/》/的/激昂/篇章/。/随着/指挥/双臂/的/挥舞/，/合唱团/的/乐音/由/震撼人心/的/齐声/合唱/，/再/到/如泣如诉/的/声部/交替/呈现/，/然后/又/回到/齐声/的/怒吼/之中/，/逐渐/加速/的/一声声/乐句/，/映出/中华/儿女/对/侵略/的/愤恨/，/最终/，/一曲/合唱/在/高昂/的/战斗/的/警/号/中/结束/，/曲/尽/而/意味深长/。/“/怒吼/吧/，/黄河/！/掀起/你/的/怒涛/，/发出/你/的/狂/叫/！/向着/全世界/的/人民/，/发出/你/的/警/号/！/”/歌声/激昂/如/磅礴/的/洪流/，/汹涌澎湃/，/激荡/人心/。/“/五千年/的/民族/，/苦难/真/不少/；/铁蹄/下/的/民众/，/苦痛/受不了/。/”/旋律/婉转/哀伤/，/道/尽/中华民族/的/苦难/与/悲愤/。/在/悲哀/愤恨/交/杂/下/，/“/新/中国/已经/破晓/，/四万万/五千万/民众/已经/团结/起来/，/誓死/同/把/国土/保/”/，/发出/了/中华民族/反/侵略/的/最强音/。/从/生疏/到/默契/，/从/胆怯/到/自信/，/从/艰难/到/成功/。/回望/在/一起/拼搏/奋斗/的/四十多天光阴/，/我们/无比/感激/有/这样/的/一个/音乐/盛会/让/我们/相聚/于/此/，/互相/帮助/，/互相/鼓励/，/一起/克服/合唱/中/的/困难/，/一起/迎接/站上/舞台/的/喜悦/。/奖杯/是/对/我们/最终/作 品/的/肯定/，/更是/对/我们/联队/团结/、/拼搏/之/精神/，/严肃/、/认真/之/态度/的/最高/评价/。/比赛/的/结束/绝不是/终点/，/我们/会/继续/保持/热情/，/继续/为/团队/的/荣誉/而/努力/奋斗/，/铭记/这份/责任/和/使命/，/不/辜负/时代/的/机遇/和/挑战/。/“/好/风/凭借力/，/送/我/上/青云/”/让/ 我们/携手/努力/，/风雨同舟/，/创造/更加/辉煌/的/明天/！
```

- 迪杰斯特拉算法

```
在/寒/冷/的/冬/日/里/，/金/色/的/长/裙/摇曳/，/浮/光/璀璨/；/深/色/的/西/装/笔/挺/，/气/宇/轩/昂/。/同/学/们/整/装/待/发/，/经/过/几个/月/的/训练/与/磨/合/，/他们/已经/成/竹/在/胸/，/静/待/在/舞/台/的/华/灯/之/下/绽/放/夺/目的/光/彩/。/下午/，/激动/人/心/的/登/台/表演/时刻/到/来/，/全/体/参/演/人员/徐/徐/登/场/，/在/聚/光/灯/之/下/开始/了/合/唱/演出/。/钢/琴/激昂/的/音/阶/倾/泻/而/出/，/朗/诵/的/声音/慷慨/激昂/，/开/启/了/《/怒/吼/吧/！/黄/河/》/的/激昂/篇/章/。/随着/指挥/双/臂/的/挥/舞/，/合/唱/团/的/乐/音/由/震撼/人/心/的/齐/声/合/唱/，/再/到/如/泣/如/诉/的/声/部/交/替/呈/现/，/然后/又/回/到/齐/声/的/怒/吼/之/中/，/逐渐/加速/的/一声/声/乐/句/，/映/出/中/华/儿/女/对/侵略/的/愤/恨/，/最终/，/一/曲/合/唱/在/高/昂/的/战斗/的/警/号/中/结束/，/曲/尽/而/意/味/深/长/。/“/怒/吼/吧/，/黄/河/！/掀起/你/的/怒/涛/，/发/出/你/的/狂/叫/！/向/着/全/世界/的/人民/，/发/出/你/的/警/号/！/”/歌/声/激昂/如/磅礴/的/洪/流/，/汹涌澎湃/，/激/荡/人/心/。/“/五/千/年/的/民族/，/苦/难/真/不/少/；/铁/蹄/下/的/民/众/，/苦/痛/受/不/了/。/”/旋/律/婉转/哀/伤/，/道/尽/中/华/民族/的/苦/难/与/悲愤/。/在/悲哀/愤/恨/交/杂/下/，/“/新/中国/已经/破/晓/，/四/万/万/五/千/万/民/众/已经/团结/起/来/，/誓/死/同/把/国/土/保/”/，/发/出/了/中/华/民族/反/侵略/的/最/强/音/。/从/生/疏/到/默/契/，/从/胆/怯/到/自/信/，/从/艰难/到/成功/。/回/望/在/一/起/拼/搏/奋斗/的/四/十/多/天/光/阴/，/我们/无/比/感激/有/这样/的/一个/音乐/盛/会/让/我们/相/聚/于/此/，/互相/帮助/，/互相/鼓励/，/一/起/克服/合/唱/中/的/困难/，/一/起/迎/接/站/上/舞/台/的/喜悦/。/奖/杯/是/对/我们/最终/作品/的/肯定/，/更/是/对/我们/联/队/团结/、/拼/搏/之/精神/，/严肃/、/认真/之/态度/的/最/高/评价/。/比赛/的/结束/绝/不/是/终/点/，/我们/会/继续/保持/热/情/，/继续/为/团/队/的/荣誉/而/努力/奋斗/，/铭/记/这/份/责任/和/使/命/，/不/辜负/时代/的/机/遇/和/挑战/。/“/好/风/凭/借/力/，/送/我/上/青/云/”/让/我们/携/手/努力/，/风/雨/同/舟/，/创造/更/加/辉煌/的/明/天/！
```

​	从效果上来看，三种匹配法要优于迪杰斯特拉算法。经过分析，原因可能在于邻接矩阵的函数选取问题，通过修改倒数的参数，发现分词结果会改变。
