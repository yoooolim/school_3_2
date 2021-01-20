#include <iostream> 
#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <sstream>
using namespace std;

//Ʈ���� ����� ���� ���
struct Node
{
 	char ch;
 	int freq;
 	Node *left, *right;
};


//��带 �ϳ� ����� ���� �Լ�
Node* makeNode(char ch, int freq, Node* left, Node* right)
{
 	Node* node = new Node();

 	node->ch = ch;
 	node->freq = freq;
 	node->left = left;
 	node->right = right;

	return node;
}

// heap�� ���ؼ� ���Ǵ� �� �Լ�
struct comp
{
 	bool operator()(Node* l, Node* r)
 	{
		// �󵵼��� ���� ���� ģ�� -> ���� ����
 		return l->freq > r->freq;
 	}
};



void encode(Node* root, string str,
 			unordered_map<char, string> &huffmanCode)
 {
 	if (root == nullptr)
 		return;

	// node�� ã�´�
 	if (!root->left && !root->right) {
 		huffmanCode[root->ch] = str;
	}	

	//�������� ���� 0, ���������� ���� 1�� encoding�Ѵ�.
 	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}


// Huffman tree ����� decode, file�� �� ����
void buildHuffmanTree(string text)
{
	//stuffing �ϱ� ���� ����
	int stuffing = 0;

	//Huffman code ����� ���� stream
	ofstream out_en;
	out_en.open("Huffman_code.hbs");

	//Huffman table ����� ���� stream
	ofstream out_ta;
	out_ta.open("Huffman_table.hbs");

/////////////////////////Tree �����//////////////////////////////////////

	// ������ character�� ��� �������� count
 	unordered_map<char, int> freq;
 	for (char ch : text) {
 		freq[ch]++;
	}

	// Huffman tree�� node���� �����ϴ� priority queue 
 	priority_queue<Node*, vector<Node*>, comp> pq;

	// ������ character�� ���ؼ� node �����ϱ� 
	for (auto pair : freq) {
 		pq.push(makeNode(pair.first, pair.second, nullptr, nullptr));
	}

	// loop (until notihing in queue)
	while (pq.size() != 1)
	{
		// ���� priority�� ������ -> �󵵼��� ����
		// �󵵼��� ���� �� �ΰ��� queue���� ������
 		Node *left = pq.top(); pq.pop();
		Node *right = pq.top();	pq.pop();

		//�� ����� ���� ���� ������ ���� ���� �� ��带 �����ϱ�
		//�� �󵵼��� ���� ���� ��忡 �����ϱ�
		//�ٽ� ��� queue�� �о� �ֱ�
 		int sum = left->freq + right->freq;
		pq.push(makeNode('\0', sum, left, right));
	}

	//root -> Huffman Tree�� root
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

	//���� 8�ڸ��� ������ �ʴ´ٸ� �������� 0���� ä���־�� �Ѵ�.
	stuffing %= 8;
	while (stuffing != 8) {
		out_ta << "0";
		stuffing++;
	}
	out_ta.close();

	//���� 8�ڸ��� ������ �ʴ´ٸ� �������� 0���� ä���־�� �Ѵ�.
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
