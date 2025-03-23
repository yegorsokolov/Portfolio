package model;

import tests.SLLNode;
import tests.TreeNode;

public class TreeUtilities {
    public SLLNode<Integer> getElementsOfRanks(TreeNode<Integer> n, int i, int j) {
        NodePair allUnsorted = collectAllElements(n);
        SLLNode<Integer> sortedHead = insertionSort(allUnsorted.head);
        SLLNode<Integer> subrangeHead = extractSubrange(sortedHead, i, j);
        return subrangeHead;
    }

    public TreeNode<String> getStats(TreeNode<Integer> n) {
        return statsHelper(n).newNode;
    }

    private static class NodePair {
        SLLNode<Integer> head;
        SLLNode<Integer> tail;
        NodePair(SLLNode<Integer> h, SLLNode<Integer> t) {
            head = h;
            tail = t;
        }
    }

    private NodePair collectAllElements(TreeNode<Integer> root) {
        SLLNode<Integer> head = new SLLNode<>(root.getElement(), null);
        SLLNode<Integer> tail = head;
        SLLNode<TreeNode<Integer>> childNode = root.getChildren();
        while (childNode != null) {
            NodePair childPair = collectAllElements(childNode.getElement());
            tail.setNext(childPair.head);
            if (childPair.tail != null) {
                tail = childPair.tail;
            }
            childNode = childNode.getNext();
        }
        return new NodePair(head, tail);
    }

    private SLLNode<Integer> insertionSort(SLLNode<Integer> head) {
        if (head == null || head.getNext() == null) {
            return head;
        }
        SLLNode<Integer> sorted = null;
        SLLNode<Integer> current = head;
        while (current != null) {
            SLLNode<Integer> nextNode = current.getNext();
            sorted = insertInOrder(sorted, current);
            current = nextNode;
        }
        return sorted;
    }

    private SLLNode<Integer> insertInOrder(SLLNode<Integer> sortedHead, SLLNode<Integer> nodeToInsert) {
        nodeToInsert.setNext(null);
        if (sortedHead == null || nodeToInsert.getElement() < sortedHead.getElement()) {
            nodeToInsert.setNext(sortedHead);
            return nodeToInsert;
        }
        SLLNode<Integer> current = sortedHead;
        while (current.getNext() != null && current.getNext().getElement() <= nodeToInsert.getElement()) {
            current = current.getNext();
        }
        nodeToInsert.setNext(current.getNext());
        current.setNext(nodeToInsert);
        return sortedHead;
    }

    private SLLNode<Integer> extractSubrange(SLLNode<Integer> head, int i, int j) {
        int skipCount = i - 1;
        SLLNode<Integer> current = head;
        while (skipCount > 0 && current != null) {
            current = current.getNext();
            skipCount--;
        }
        SLLNode<Integer> subrangeHead = current;
        int steps = j - i;
        while (steps > 0 && current != null) {
            current = current.getNext();
            steps--;
        }
        if (current != null) {
            current.setNext(null);
        }
        return subrangeHead;
    }

    private static class StatsResult {
        TreeNode<String> newNode;
        int count;
        int sum;
        StatsResult(TreeNode<String> node, int c, int s) {
            newNode = node;
            count = c;
            sum = s;
        }
    }

    private StatsResult statsHelper(TreeNode<Integer> root) {
        TreeNode<String> newNode = new TreeNode<>(null);
        int totalCount = 1;
        int totalSum = root.getElement();
        SLLNode<TreeNode<Integer>> child = root.getChildren();
        while (child != null) {
            StatsResult childResult = statsHelper(child.getElement());
            newNode.addChild(childResult.newNode);
            childResult.newNode.setParent(newNode);
            totalCount += childResult.count;
            totalSum += childResult.sum;
            child = child.getNext();
        }
        String label = String.format("Number of descendants: %d; Sum of descendants: %d", totalCount, totalSum);
        newNode.setElement(label);
        return new StatsResult(newNode, totalCount, totalSum);
    }
}
