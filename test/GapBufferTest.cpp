#include "gtest/gtest.h"

#include "GapBuffer.h"

TEST(GapBuffer, test_constructor_01) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	ASSERT_NE(0, gb.capacity());
	ASSERT_LT(gb.size(), gb.capacity());
}

TEST(GapBuffer, test_constructor_02) {
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

TEST(GapBuffer, test_insert_01) {
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

TEST(GapBuffer, test_insert_02) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int num = 200;
	int rc = 0;
	while (rc != num) {
		rc = gb.insert(rc, rc);
	}
	ASSERT_EQ(num, gb.size());
	ASSERT_GT(gb.capacity(), gb.size());
	for (int i = 0; i < gb.size(); i++) {
		ASSERT_EQ(i, gb[i]);
	}
}

TEST(GapBuffer, test_insert_03) {
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

TEST(GapBuffer, test_insert_04) {
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

TEST(GapBuffer, test_insert_06) {
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


TEST(GapBuffer, test_insert_07) {
	GapBuffer<int> gb;
	ASSERT_EQ(0, gb.size());
	int pos = 0;
	int val = 123;
	int num = 1;
	gb.insert(pos, num, val);
	ASSERT_EQ(num, gb.size());
	ASSERT_EQ(val, gb[0]);
}

TEST(GapBuffer, test_insert_08) {
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

TEST(GapBuffer, test_insert_09) {
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

TEST(GapBuffer, test_insert_10) {
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

TEST(GapBuffer, test_insert_11) {
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

TEST(GapBuffer, test_insert_12) {
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

TEST(GapBuffer, test_insert_13) {
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

TEST(GapBuffer, test_insert_14) {
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

TEST(GapBuffer, test_insert_15) {
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

TEST(GapBuffer, test_insert_16) {
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


