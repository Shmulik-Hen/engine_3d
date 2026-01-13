# Branch Deletion Guide

## SDL2 Dev Branch Status

**Investigation Date:** January 2026

### Current Status
After thorough investigation of the repository:
- **No "sdl2 dev branch" currently exists** in the local or remote repository
- Only branch visible: `copilot/delete-sdl2-dev-branch`
- The SDL2 code exists as part of the main codebase in `src/graphics_sdl2.cpp`

### How to Delete a Branch

If you need to delete a branch in the future, here are the commands:

#### Delete a Local Branch
```bash
# Delete a local branch (must not be checked out)
git branch -d <branch-name>

# Force delete a local branch (even if not fully merged)
git branch -D <branch-name>
```

#### Delete a Remote Branch
```bash
# Delete a branch from the remote repository
git push origin --delete <branch-name>

# Alternative syntax
git push origin :<branch-name>
```

#### Delete Both Local and Remote
```bash
# Delete local branch
git branch -d <branch-name>

# Delete remote branch
git push origin --delete <branch-name>
```

### Example: Deleting a Hypothetical "sdl2-dev" Branch

If an "sdl2-dev" branch existed, you would delete it with:

```bash
# Switch to a different branch first (e.g., main or master)
git checkout main

# Delete the local sdl2-dev branch
git branch -d sdl2-dev

# Delete the remote sdl2-dev branch
git push origin --delete sdl2-dev
```

### Verification

To verify a branch has been deleted:

```bash
# List all local branches
git branch

# List all remote branches
git branch -r

# List all branches (local and remote)
git branch -a
```

## Conclusion

The "sdl2 dev branch" either:
1. Never existed in this repository
2. Was already deleted in the past
3. Exists under a different name

The SDL2 graphics implementation is part of the main codebase and is not on a separate development branch.
