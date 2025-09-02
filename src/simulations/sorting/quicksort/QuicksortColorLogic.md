# QuickSort Visualization Color Logic (Hoare Partitioning)

## Overview
This document explains the color coding system used in the proper QuickSort visualization using Hoare partitioning scheme, designed to help users understand the two-pointer algorithm execution.

## Algorithm: Hoare Partitioning QuickSort
This implementation uses the correct two-pointer approach where:
- **Left pointer** scans rightward for elements ≥ pivot
- **Right pointer** scans leftward for elements ≤ pivot  
- **Elements are swapped** when both pointers find targets
- **Pivot is placed** in final position after partitioning

## Color Hierarchy (All Green Shades)

### 1. Brightest Green (0, 255, 65) - "ACTIVE OPERATION"
**When Used**: Current algorithm action happening RIGHT NOW
**QuickSort Context**: 
- **During Pointer Scanning**: Left and right pointers actively scanning for elements
- **During Swaps**: Both elements being swapped with each other
- **During Pivot Placement**: Elements involved in final pivot positioning
**Purpose**: Show exactly what the algorithm is doing at this moment

### 2. Medium Green (100, 255, 100) - "PIVOT REFERENCE" 
**When Used**: The current pivot element (first element in partition)
**QuickSort Context**: The reference value that determines left/right partitioning
**Purpose**: Provide constant visual reference for all comparisons

### 3. Dim Green (0, 102, 0) - "INACTIVE"
**When Used**: All other elements not currently involved
**QuickSort Context**: Elements not being scanned, swapped, or serving as pivot
**Purpose**: De-emphasize irrelevant elements

**Note**: Range highlighting has been REMOVED to eliminate visual clutter

## QuickSort Hoare Partitioning Steps

### Step 1: Pivot Selection & Pointer Setup
- **Pivot** (first element): Medium Green
- **Left pointer** (after pivot): Brightest Green
- **Right pointer** (last element): Brightest Green
- **Rest**: Dim Green

### Step 2: Left Pointer Scanning
- **Left pointer** (current scan position): Brightest Green
- **Pivot**: Medium Green
- **Rest**: Dim Green

### Step 3: Right Pointer Scanning  
- **Right pointer** (current scan position): Brightest Green
- **Pivot**: Medium Green
- **Rest**: Dim Green

### Step 4: Swap Operation
- **Both elements being swapped**: Brightest Green
- **Pivot**: Medium Green
- **Rest**: Dim Green

### Step 5: Pivot Final Placement
- **Pivot moving to final position**: Brightest Green
- **Rest**: Dim Green

## Visual Design Principles

1. **Focused Highlighting**: Only active pointers/operations get brightest color
2. **No Range Clutter**: Removed background range highlighting for clarity
3. **Clear Two-Pointer Logic**: Left and right scanning clearly visible
4. **Partition Boundaries**: Implicit through pointer movement, not excessive highlighting
5. **Minimal Distraction**: Maximum focus on current algorithm action

## Key Improvements Over Previous Version

- **Correct Algorithm**: Hoare partitioning instead of incorrect Lomuto
- **Two-Pointer Visualization**: Shows proper left/right scanning
- **Reduced Visual Noise**: No more excessive range highlighting  
- **Focused Operations**: Only current action highlighted in brightest green
- **Proper Partitioning**: Clear left/right division around pivot

## Implementation Notes

Color selection based on step descriptions:
- "Left scan" / "Right scan" → Brightest green for active pointer
- "Will swap" / "Swapped" → Brightest green for swap elements
- "Moving pivot" → Brightest green for pivot placement
- Pivot index → Always medium green reference
- No range highlighting → Clean, focused visualization