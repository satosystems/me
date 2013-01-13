#include "gtest/gtest.h"

#include "GapBuffer.h"

TEST(GapBuffer, test_constructor_a_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	ASSERT_NE(0, gb.capacity());
	ASSERT_LT(gb.size(), gb.capacity());
}

TEST(GapBuffer, test_constructor_b_01) {
	const int growthSize = 4;
	GapBuffer<int> gb(growthSize);
	ASSERT_EQ(0, gb.size());
	ASSERT_EQ(growthSize, gb.capacity());
	int val = 123;
	for (int i = 0; i < growthSize; i++) {
		gb.push_back(val);
	}
	ASSERT_EQ(growthSize, gb.capacity());
	gb.push_back(val);
	ASSERT_LT(growthSize, gb.capacity());
	ASSERT_LE(growthSize * 2, gb.capacity());
}

TEST(GapBuffer, test_size_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int val = 123;
	gb.push_back(val);
	ASSERT_EQ(1, gb.size());
}

TEST(GapBuffer, test_size_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	gb.insert(pos, val);
	ASSERT_EQ(1, gb.size());
}

TEST(GapBuffer, test_size_03) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int num = 2;
	int val = 123;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
}

TEST(GapBuffer, test_size_04) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int num = 200;
	int val = 123;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
}

TEST(GapBuffer, test_size_05) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int num = 3;
	int array[num];
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num, gb.size());
}

TEST(GapBuffer, test_size_06) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int num = 200;
	int array[num];
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num, gb.size());
}

TEST(GapBuffer, test_capacity_01) {
	const int growthSize = 1;
	GapBuffer<int> gb(growthSize);
	ASSERT_EQ(0, gb.size());
	int capacity = gb.capacity();
	ASSERT_EQ(growthSize, capacity);
	int val = 123;
	gb.push_back(val);
	ASSERT_EQ(growthSize, gb.capacity()); // not yet grow
	gb.push_back(val);
	ASSERT_LT(growthSize * 2, gb.capacity()); // has been grown
}

TEST(GapBuffer, test_capacity_02) {
	const int growthSize = 1;
	GapBuffer<int> gb(growthSize);
	ASSERT_EQ(0, gb.size());
	int capacity = gb.capacity();
	ASSERT_EQ(growthSize, capacity);
	int pos = 0;
	int val = 123;
	int num = 100;
	gb.insert(pos , num, val);
	ASSERT_EQ(num, gb.size());
	ASSERT_LT(growthSize * num, gb.capacity());
	ASSERT_LT(gb.size(), gb.capacity());
}

TEST(GapBuffer, test_clear_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	gb.insert(pos, val);
	ASSERT_EQ(1, gb.size());
	gb.clear();
	ASSERT_EQ(0, gb.size());
}

TEST(GapBuffer, test_insert_a_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.insert(i, i);
	}
	ASSERT_EQ(num, gb.size());
	ASSERT_GT(gb.capacity(), gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_a_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int pos = 0;
	while (pos != num) {
		pos = gb.insert(pos, pos);
	}
	ASSERT_EQ(num, gb.size());
	ASSERT_GT(gb.capacity(), gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_a_03) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int pos = 0;
	for (int i = 0; i < num; i++) {
		gb.insert(pos, i);
	}
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(num - i - 1, gb[i]);
	}
}

TEST(GapBuffer, test_insert_a_04) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.insert(i, gb.size());
	}
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_a_05) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	gb.insert(0, 100);
	gb.insert(1, 200);
	gb.insert(1, 150);
	ASSERT_EQ(3, gb.size());
	ASSERT_EQ(100, gb[0]);
	ASSERT_EQ(150, gb[1]);
	ASSERT_EQ(200, gb[2]);
}


TEST(GapBuffer, test_insert_b_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	int num = 1;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
	ASSERT_EQ(val, gb[0]);
}

TEST(GapBuffer, test_insert_b_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	int num = 10;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(val, gb[i]);
	}
}

TEST(GapBuffer, test_insert_b_03) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	int num = 200;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(val, gb[i]);
	}
}

TEST(GapBuffer, test_insert_b_04) {
	const int growthSize = 1;
	GapBuffer<int> gb(growthSize);
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	int num = 200;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(val, gb[i]);
	}
}

TEST(GapBuffer, test_insert_b_05) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	gb.push_back(0);
	gb.push_back(1);
	int pos = 1;
	int val = 123;
	int num = 10;
	gb.insert(pos, num, val);
	ASSERT_EQ(num + 2, gb.size());
	ASSERT_EQ(0, gb[0]);
	for (int i = pos; i < num; i++) {
		ASSERT_EQ(val, gb[i]);
	}
	ASSERT_EQ(1, gb[gb.size() - 1]);
}

TEST(GapBuffer, test_insert_b_06) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	gb.push_back(0);
	gb.push_back(1);
	int pos = 1;
	int val = 123;
	int num = 200;
	gb.insert(pos, num, val);
	ASSERT_EQ(num + 2, gb.size());
	ASSERT_EQ(0, gb[0]);
	for (int i = pos; i < num; i++) {
		ASSERT_EQ(val, gb[i]);
	}
	ASSERT_EQ(1, gb[gb.size() - 1]);
}

TEST(GapBuffer, test_insert_b_07) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val1 = 123;
	int val2 = 456;
	int num = 200;
	pos = gb.insert(pos, num, val1);
	ASSERT_EQ(num, gb.size());
	ASSERT_EQ(pos, gb.size());
	pos = gb.insert(pos, num, val2);
	ASSERT_EQ(num * 2, gb.size());
	ASSERT_EQ(pos, gb.size());
	for (int i = 0; i < num; i++) {
		ASSERT_EQ(val1, gb[i]);
	}
	for (int i = num; i < gb.size(); i++) {
		ASSERT_EQ(val2, gb[i]);
	}
}

TEST(GapBuffer, test_insert_c_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 10;
	int pos = 0;
	int array[num];
	for (int i = 0; i < num; i++) {
		array[i] = i;
	}
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_c_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int pos = 0;
	int array[num];
	for (int i = 0; i < num; i++) {
		array[i] = i;
	}
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_c_03) {
	const int growthSize = 1;
	GapBuffer<int> gb(growthSize);
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int pos = 0;
	int array[num];
	for (int i = 0; i < num; i++) {
		array[i] = i;
	}
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num, gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_c_04) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	gb.push_back(-1);
	gb.push_back(-2);
	int num = 200;
	int pos = 1;
	int array[num];
	for (int i = 0; i < num; i++) {
		array[i] = pos + i;
	}
	gb.insert(pos, array, array + num);
	ASSERT_EQ(num + 2, gb.size());
	ASSERT_EQ(-1, gb[0]);
	for (int i = pos; i < num; i++) {
		ASSERT_EQ(i, gb[i]);
	}
	ASSERT_EQ(-2, gb[gb.size() - 1]);
}

TEST(GapBuffer, test_insert_c_05) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int pos = 0;
	int array1[num];
	for (int i = 0; i < num; i++) {
		array1[i] = + i;
	}
	int array2[num];
	for (int i = 0; i < num; i++) {
		array2[i] = i + num;
	}
	pos = gb.insert(pos, array1, array1 + num);
	ASSERT_EQ(num, gb.size());
	ASSERT_EQ(pos, gb.size());
	pos = gb.insert(pos, array2, array2 + num);
	ASSERT_EQ(num * 2, gb.size());
	ASSERT_EQ(pos, gb.size());
	for (int i = 0; i < num; i++) {
		ASSERT_EQ(i, gb[i]);
	}
	for (int i = num; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_push_back_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 10;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_push_back_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_erase_a_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	for (int i = 0; i < num; i++) {
		ASSERT_EQ(num - i, gb.size());
		gb.erase(0);
		ASSERT_EQ(num - i - 1, gb.size());
	}
}

TEST(GapBuffer, test_erase_a_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	for (int i = 0; i < num; i++) {
		ASSERT_EQ(num - i, gb.size());
		gb.erase(gb.size() - 1);
		ASSERT_EQ(num - i - 1, gb.size());
	}
}

TEST(GapBuffer, test_erase_a_03) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	gb.push_back(0);
	gb.push_back(1);
	gb.push_back(2);
	ASSERT_EQ(3, gb.size());
	gb.erase(1);
	ASSERT_EQ(2, gb.size());
	ASSERT_EQ(0, gb[0]);
	ASSERT_EQ(2, gb[1]);
}

TEST(GapBuffer, test_erase_b_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	gb.erase(0, gb.size());
	ASSERT_EQ(0, gb.size());
}

TEST(GapBuffer, test_erase_b_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	gb.erase(0, num / 2);
	ASSERT_EQ(num / 2, gb.size());
	for (int i = 0; i < num / 2; i++) {
		ASSERT_EQ(num / 2 + i, gb[i]);
	}
}

TEST(GapBuffer, test_erase_b_03) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	gb.erase(num / 2, num);
	ASSERT_EQ(num / 2, gb.size());
	for (int i = 0; i < num / 2; i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_erase_b_04) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	const int step = 2;
	for (int i = 0; gb.size() != 0; i++) {
		ASSERT_EQ(i * step, gb[0]);
		ASSERT_EQ(num - i * step, gb.size());
		gb.erase(0, step);
		ASSERT_EQ(num - (i + 1) * step, gb.size());
	}
}

TEST(GapBuffer, test_erase_b_05) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	for (int i = 0; i < num; i++) {
		gb.push_back(i);
		ASSERT_EQ(i + 1, gb.size());
		ASSERT_EQ(i, gb[i]);
	}
	const int step = 10;
	for (int i = 0; gb.size() != 0; i++) {
		ASSERT_EQ(i * step, gb[0]);
		ASSERT_EQ(num - i * step, gb.size());
		gb.erase(0, step);
		ASSERT_EQ(num - (i + 1) * step, gb.size());
	}
}

