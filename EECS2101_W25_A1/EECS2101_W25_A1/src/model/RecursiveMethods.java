package model;

import java.util.ArrayList;
import java.util.HashSet;

public class RecursiveMethods {

    public String task1(String in_string) {
        return task1Helper(in_string, 0, -1);
    }

    private String task1Helper(String str, int index, int openIndex) {
        if (index >= str.length()) return "";
        char ch = str.charAt(index);

        if (ch == '(') {
            return task1Helper(str, index + 1, index);
        } else if (ch == ')' && openIndex != -1) {
            return str.substring(openIndex, index + 1);
        }
        return task1Helper(str, index + 1, openIndex);
    }

    public boolean task2(int[] a, int target) {
        return task2Helper(a, target, 0);
    }

    private boolean task2Helper(int[] a, int target, int index) {
        if (target == 0) return true;
        if (index >= a.length) return false;
        return task2Helper(a, target - a[index], index + 1) || task2Helper(a, target, index + 1);
    }

    public int task3(int h, int n) {
        if (h == 0) {
            return 1;
        }
        if (h < 0) {
            return 0;
        }

        int ways = 0;
        for (int i = 1; i <= n; i++) {
            ways += task3(h - i, n);
        }
        return ways;
    }

    public HashSet<ArrayList<Integer>> task4(int h, int n) {
        HashSet<ArrayList<Integer>> result = new HashSet<>();
        generateStrategies(h, n, new ArrayList<>(), result);
        return result;
    }

    private void generateStrategies(int h, int n, ArrayList<Integer> current, HashSet<ArrayList<Integer>> result) {
        if (h == 0) {
            result.add(new ArrayList<>(current));
            return;
        }
        if (h < 0) {
            return;
        }

        for (int i = 1; i <= n; i++) {
            current.add(i);
            generateStrategies(h - i, n, current, result);
            current.remove(current.size() - 1);
        }
    }
}
