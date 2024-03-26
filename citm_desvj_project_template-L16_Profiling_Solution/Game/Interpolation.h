
void Lerp(int &value, float t, int destination) {
	value += (t) * (destination - value);
}