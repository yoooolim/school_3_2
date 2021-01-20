#include <iostream> 
#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <bitset>
using namespace std;

//트리를 만들기 위한 노드
struct Node
{
	char ch;
	int freq;
	Node *left, *right;
};


//노드를 하나 만들기 위한 함수
Node* makeNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// heap을 위해서 사용되는 비교 함수
struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// 빈도수가 가장 낮은 친구 -> 높은 순위
		return l->freq > r->freq;
	}
};


void decode(Node* root, int &index, string str,ofstream &out)
{
 	if (root == nullptr) {
 		return;
	}

 	// node를 찾는다
 	if (!root->left && !root->right)
 	{
 		out << root->ch;
 		return;
 	}

 	index++;

	//재귀 함수로 노드를 돈다
 	if (str[index] == '0')
 		decode(root->left, index, str,out);
	else
 		decode(root->right, index, str,out);
}



// Huffman tree 만들고 decode, file에 값 저장
void buildHuffmanTree(string text)
{
	//Huffman code 파일
	ifstream code;
	code.open("Huffman_code.hbs");

	//decode한 값들을 저장하기 위한 파일
	ofstream out;
	out.open("Output.txt");

	//stuffing을 위한 변수
	int stuffing = 0; 
	int count = 0;
	char buf[1000000] = { NULL,};
	string str = "";

/////////////////////////Tree 만들기//////////////////////////////////////

	// 각각의 character가 몇번 나오는지 count
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}

	// Huffman tree의 node들을 저장하는 priority queue
	priority_queue<Node*, vector<Node*>, comp> pq;

	// 나오는 character에 관해서 node 생성하기 
	for (auto pair : freq) {
		pq.push(makeNode(pair.first, pair.second, nullptr, nullptr));
	}

	// loop (until notihing in queue)
	while (pq.size() != 1)
	{
		// 높은 priority를 가졌다 -> 빈도수가 낮다
		// 빈도수가 낮은 것 두개를 queue에서 빼내기
		Node *left = pq.top(); pq.pop();
		Node *right = pq.top();	pq.pop();

		//한 노드의 왼쪽 노드와 오른쪽 노드로 위의 두 노드를 지정하기
		//두 빈도수의 합을 상위 노드에 저장하기
		//다시 노드 queue로 밀어 넣기
		int sum = left->freq + right->freq;
		pq.push(makeNode('\0', sum, left, right));
	}

	//root -> Huffman Tree의 root
	Node* root = pq.top();

/////////////////////get Huffman code//////////////////////////	
	code.seekg(0, ios::end);

	//tellg() -> get size
	int sz = code.tellg();

	//seekg() -> back pointer to first
	code.seekg(0, ios::beg);

	//binary file -> read
	code.read(buf, sz);

	while (buf[count] != NULL) {
		str += buf[count];
		count++;
	}

	int index = -1;
	while (index < (int)str.size() - 2) {
 		decode(root, index, str,out);
	}

}

int main()
{
	//input file 열기
	ifstream in("input.txt");
	string text;
	string all;
	int count = 0;

	if (in.fail()) {
		cout << "failed _ file open" << endl;
		return -1;
	}

	while (!in.eof()) {
		getline(in, text);
		count++;
		all += text;
	}
	buildHuffmanTree(all);

	return 0;
}
