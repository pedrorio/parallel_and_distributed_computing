#ifndef MPIGRID_CELL_H
#define MPIGRID_CELL_H

// The slice of the problem owned by one rank: its user-row block, its item-column
// block, and the non-zeros that fall in that grid cell (stored with LOCAL indices,
// i.e. relative to userStart / itemStart).
struct Cell {
    int userStart, uLocal;   // user-row block  [userStart, userStart + uLocal)
    int itemStart, iLocal;   // item-col block  [itemStart, itemStart + iLocal)
    int nnz;                 // non-zeros owned by this rank
    int *localUser;          // local user index of each non-zero
    int *localItem;          // local item index of each non-zero
    double *value;           // its A value
};

#endif  // MPIGRID_CELL_H
