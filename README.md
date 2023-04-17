# kapable-clang-sast
This project aims to automatically document Linux capability calls capable() with Clang AST

Currently, the usual criteria for managing privileges on operating systems do not allow compliance with regulations such as the RGPD or export control rules. Indeed, administrators can override security mechanisms by granting inappropriate privileges for their administrative tasks. In the case of multi-administrator systems, the objective is to eliminate trust agreements between co-administrators, to minimise the risks associated with these agreements and thus to better comply with zero-trust policies.

The Linux kernel documentation is incomplete with regard to the privileges required to use some of its features. For any admin, or most of developers, capabilities are unknown. I'd say only hackers or kernel developers really know their existence and their scope.

To solve the lack of documentation, I tried to analyze the kernel to "map" the capability requirements implied by system calls and describe them automatically with trees. This work could help explain why a privilege is needed or not, regardless of the kernel version. This way system administrator could know the scope of action through SAST or DAST, then configure his co-administrative policy to these scopes.

Currently, the solution for administrators is to use this RootAsRole eBPF which could detect capabilities asked by program. But this eBPF returns false-positives. With more context (e.g. the name of a syscall or the context of caller symbol), this eBPF could filter impertinent privileges asks, and simplify configuration of privileges for administrators.

I had two approach to "map" capabilities:

    If you take the kernel source code, you can see capability asks with capable() function call. By SAST (call-graph like), you could determine the map of privilege. Then, any distribution could create better documentation for their kernel target.

    If you take kernel image elf into IDA or Ghidra and retrieve capable() call symbol, you could map privilege with third party program. By automating process, you could determinate map regardless of any kernel.

This repo is source code about what I tried to do with the time that I had.

Note: I'm not a C++ developer at all. So The few I tried to do is huge time to learn the language.
