#include <iostream> 
#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <sstream>
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



void encode(Node* root, string str,
 			unordered_map<char, string> &huffmanCode)
 {
 	if (root == nullptr)
 		return;

	// node를 찾는다
 	if (!root->left && !root->right) {
 		huffmanCode[root->ch] = str;
	}	

	//왼쪽으로 가면 0, 오른쪽으로 가면 1로 encoding한다.
 	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}


// Huffman tree 만들고 decode, file에 값 저장
void buildHuffmanTree(string text)
{
	//stuffing 하기 위한 변수
	int stuffing = 0;

	//Huffman code 출력을 위한 stream
	ofstream out_en;
	out_en.open("Huffman_code.hbs");

	//Huffman table 출력을 위한 stream
	ofstream out_ta;
	out_ta.open("Huffman_table.hbs");

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

	/////////////////////ENCODING/////////////////////////	
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);


	stuffing = 0;
 	for (auto pair : huffmanCode) {
		cout << pair.first << "	" << pair.second << endl;
		out_ta << bitset<8>(pair.first) << bitset<8>(pair.second.length()) << pair.second ;
		stuffing += 8 + pair.second.length();
	}

	//만약 8자리로 끝나지 않는다면 마지막을 0으로 채워주어야 한다.
	stuffing %= 8;
	while (stuffing != 8) {
		out_ta << "0";
		stuffing++;
	}
	out_ta.close();

	//만약 8자리로 끝나지 않는다면 마지막을 0으로 채워주어야 한다.
 	string str = "";
	stuffing = 0;
	int num = 0;
 	for (char ch : text) {
 		str+=huffmanCode[ch];
		stuffing += huffmanCode[ch].length();
	}
	cout << stuffing;
	stuffing %= 8;
	while (stuffing != 8) {
		str+= "0";
		stuffing++;
	}
	std::stringstream sstream(str);
	while (sstream.good()) {
		bitset<8> bits;
		sstream >> bits;
		char c = char(bits.to_ulong());
		out_en << c;
	}
	out_en.close();
}


int main()
{
	ifstream in("input.txt");
	string text;
	string all;
	int count=0;

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
