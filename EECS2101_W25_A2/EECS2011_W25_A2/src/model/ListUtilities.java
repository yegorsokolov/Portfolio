package model;
import tests.Node;

public class ListUtilities {
    
    public Node<String> getAllPrefixes(Node<Integer> input, int m, int n) {
        Node<String> head = null, tail = null;
        Node<Integer> current = input;
        StringBuilder prefix = new StringBuilder();
        int count = 0;
        
        while (current != null) {
            if (count > 0) {
                prefix.append(", ");
            }
            prefix.append(current.getElement());
            count++;
            
            if (count >= m && count <= n) {
                Node<String> newNode = new Node<>("[" + prefix.toString() + "]", null);
                if (head == null) {
                    head = newNode;
                } else {
                    tail.setNext(newNode);
                }
                tail = newNode;
            }
            current = current.getNext();
        }
        return head;
    }
    
    public Node<Integer> getMergedChain(Node<Integer> left, Node<Integer> right) {
        Node<Integer> dummy = new Node<>(0, null);
        Node<Integer> tail = dummy;
        
        while (left != null && right != null) {
            if (left.getElement() <= right.getElement()) {
                tail.setNext(new Node<>(left.getElement(), null));
                left = left.getNext();
            } else {
                tail.setNext(new Node<>(right.getElement(), null));
                right = right.getNext();
            }
            tail = tail.getNext();
        }
        
        while (left != null) {
            tail.setNext(new Node<>(left.getElement(), null));
            left = left.getNext();
            tail = tail.getNext();
        }
        
        while (right != null) {
            tail.setNext(new Node<>(right.getElement(), null));
            right = right.getNext();
            tail = tail.getNext();
        }
        
        return dummy.getNext();
    }
    
    public Node<Integer> getInterleavedArithmeticFibonacciSequences(int start, int diff, int sizeA, int sizeF) {
        Node<Integer> head = null, tail = null;
        int aVal = start, f1 = 1, f2 = 1;
        boolean isArithmeticTurn = true;
        
        while (sizeA > 0 || sizeF > 0) {
            int value;
            if (isArithmeticTurn && sizeA > 0) {
                value = aVal;
                aVal += diff;
                sizeA--;
            } else if (!isArithmeticTurn && sizeF > 0) {
                value = f1;
                int temp = f1 + f2;
                f1 = f2;
                f2 = temp;
                sizeF--;
            } else {
                isArithmeticTurn = !isArithmeticTurn;
                continue;
            }
            
            Node<Integer> newNode = new Node<>(value, null);
            if (head == null) {
                head = newNode;
            } else {
                tail.setNext(newNode);
            }
            tail = newNode;
            isArithmeticTurn = !isArithmeticTurn;
        }
        return head;
    }
    
    public Node<String> getGroupedStrings(Node<String> input, int m, int n) {
        Node<String> head = null, tail = null;
        Node<String> current = input;
        Node<String> group1 = null, group2 = null, group3 = null;
        Node<String> tail1 = null, tail2 = null, tail3 = null;
        
        while (current != null) {
            String str = current.getElement();
            int len = str.length();
            
            Node<String> newNode = new Node<>(str, null);
            if (len < m) {
                if (group1 == null) {
                    group1 = newNode;
                } else {
                    tail1.setNext(newNode);
                }
                tail1 = newNode;
            } else if (len < n) {
                if (group2 == null) {
                    group2 = newNode;
                } else {
                    tail2.setNext(newNode);
                }
                tail2 = newNode;
            } else {
                if (group3 == null) {
                    group3 = newNode;
                } else {
                    tail3.setNext(newNode);
                }
                tail3 = newNode;
            }
            current = current.getNext();
        }
        
        head = mergeGroups(group1, group2, group3);
        return head;
    }
    
    private Node<String> mergeGroups(Node<String> g1, Node<String> g2, Node<String> g3) {
        Node<String> head = g1 != null ? g1 : (g2 != null ? g2 : g3);
        Node<String> tail = head;
        
        while (tail != null && tail.getNext() != null) {
            tail = tail.getNext();
        }
        if (tail != null) tail.setNext(g2);
        while (tail != null && tail.getNext() != null) {
            tail = tail.getNext();
        }
        if (tail != null) tail.setNext(g3);
        
        return head;
    }
}
