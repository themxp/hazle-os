# Prompt for AI Agent — Implement FAT32-like Filesystem & Block Device I/O

**Goal:**
Buat implementasi modul filesystem (mulai dari read-only sampai full read/write/delete/rename/lfn) untuk OS baru. Fokus utama: **FAT32-style on-disk format** (BPB, FAT, dir entries, LFN) **dan komunikasi yang andal antara OS dan perangkat block (hard-drive/SSD/virtio)** sehingga operasi file (read, copy, write, delete, list, extract) berfungsi di lingkungan nyata dan virtual (QEMU).

> Catatan: agen boleh menggunakan referensi manapun (Microsoft FAT spec, OSDev, Linux kernel docs, implementasi open-source) untuk memastikan kompatibilitas dan ketepatan on-disk format dan algoritma.

---

## Kewajiban (Hard Requirements)

1. **Compatibility:** Implementasi harus membaca dan menulis **FAT32 on-disk structures** (BPB fields, FAT entries 28-bit semantics, dir entries, LFN). Volume yang dibuat oleh implementasi harus dapat dibaca oleh Windows / Linux `mount` (FAT32) dan sebaliknya.
2. **Block device abstraction:** Agen harus menggunakan antarmuka block-level: `read_blocks(lba, n, buf)` dan `write_blocks(lba, n, buf)` (synch API). Semua operasi filesystem harus dibangun di atas API ini.
3. **Core features (MVP):** mount, list directory, read file, create file, write file, delete file, rename, support LFN, basic timestamps, attributes (readonly/hidden/system/archive).
4. **Consistency semantics:** implementasi harus menerapkan urutan penulisan yang meminimalkan korupsi (mis. allocate clusters → write data → update FAT → create directory entry OR allocate FAT → write data → flush → write dir entry) dan expose `fsync(fd)` untuk memaksa flush ke device.
5. **Multiple FAT copies:** saat menulis FAT, pastikan menulis semua salinan FAT pada disk.
6. **Free cluster management:** maintain `last_allocated` pointer untuk alokasi cepat dan implementasikan free-chain reclamation saat delete.
7. **Crash-safety testing:** agen harus menyediakan test plan untuk mensimulasikan power-loss/crash pada berbagai titik (setiap langkah penting) dan strategi recovery (simple fsck tool) untuk menemukan dan mengembalikan cluster leaks.

---

## Desain teknis yang diharapkan

Berikan desain tingkat-tinggi **dan** rincian implementasi (pseudocode/C snippets) untuk fungsi-fungsi inti berikut:

1. **BPB parsing & initialization**

   * Baca BPB dari boot sector, hitung `bytes_per_sector`, `sectors_per_cluster`, `reserved_sectors`, `num_fats`, `sectors_per_fat`, `first_data_sector`, `root_cluster`.
   * Ekspos struktur `fs_info` dengan semua parameter yang dibutuhkan.

2. **Cluster ↔ LBA konversi**

   * Implementasikan `cluster_to_lba(cluster)` rumus: `first_data_sector + (cluster - 2) * sectors_per_cluster`.

3. **FAT access layer**

   * Abstraksi baca/tulis FAT entry (28-bit mask), cache FAT sector(s) untuk performa.
   * Menangani EOC ranges (`0x0FFFFFF8..0x0FFFFFFF`) dan reserved values.

4. **Directory parser & LFN**

   * Parser entri direktori 32-byte dan LFN entries (0x0F attr), reconstruct UTF-16 LFN → UTF-8.
   * Cari entri kosong (`0x00`) dan entri terhapus (`0xE5`).

5. **File read algorithm**

   * Traverse cluster chain via FAT entries, copy content ke user buffer, riset final partial cluster.

6. **File write & allocation**

   * `allocate_clusters(n)`, set chain in FAT, write data to clusters, mark EOC on final cluster.
   * Update dir entry atomically (or as-atomic-as-possible) dan flush.

7. **Delete**

   * Mark dir entry `0xE5` dan free chain in FAT (set 0).
   * Bahas urutan operasi dan tradeoffs untuk safety.

8. **Rename / move**

   * Update directory entries (possibly across directories), memindahkan dir entry dan menjaga timestamps.

9. **Utilities**

   * `find_free_cluster`, `count_free_clusters`, `walk_directory(cluster)`, `create_dir_entry(parent_cluster, name, first_cluster, size)`.

10. **Performance & caching**

    * Block cache design (read cache + delayed write-back), batch FAT updates, periodic flush.

---

## OS ↔ Hardware (Block Device) Requirements — TEKANKAN PADA AGENT

Agen harus memastikan OS berkomunikasi dengan storage **via proper block device driver**. Dokumentasikan & implementasikan (atau mock) langkah‑langkah berikut:

1. **Abstraksi Device:** gunakan API `read_blocks(lba, cnt, buf)` dan `write_blocks(lba, cnt, buf)` yang mengandalkan driver perangkat (SATA/AHCI, NVMe, USB-mass-storage, atau virtio-blk). Semua operasi FS harus terjadi melalui API ini.
2. **DMA / Interrupts / Completion:** jika mengimplementasikan low-level driver, pastikan penanganan DMA submission dan completion interrupt; jika menggunakan hypervisor (QEMU), gunakan virtio or emulated block devices.
3. **Ordering & Barriers:** agen harus memastikan `fsync`/barrier semantics — gunakan cache flush/barrier primitives di driver agar urutan penulisan dijamin (mis. write FAT sector → flush device cache → write dir entry). Jelaskan bagaimana memanggil `fsync` pada driver level.
4. **Alignment & sector size:** Pastikan semua I/O aligned ke `bytes_per_sector` dan gunakan sizes yang kompatibel dengan device (512/4096).
5. **Handling device errors:** timeouts, transient errors, I/O retries, dan merked recovery (bad sector handling) harus didesain.
6. **Testing on real devices & QEMU:** berikan instruksi untuk testing dengan `losetup` + mount di Linux dan QEMU virtio-blk + SeaBIOS (BIOS) atau UEFI for safety.

---

## Tes & Verifikasi (deliverables)

Agen harus menyertakan script/testcase yang otomatis memverifikasi:

1. **Mountability test:** image created can be mounted on Linux/Windows and files visible.
2. **IO tests:** read/write/delete/copy large files (including <4GB limit tests), LFN names, attribute persistence.
3. **Crash-sim tests:** inject crash after: (a) allocate clusters, (b) after writing data but before dir update, (c) after dir entry but before FAT update. Provide `fsck` procedure outputs.
4. **Performance microbench:** sequential read/write throughput and random small writes.
5. **Edge cases:** full disk, fragmented files, very long LFN sequences, deleted and recreated names.

---

## Output yang diharapkan dari agen (format deliverable)

1. `fat32_driver.c` / `fat32_driver.h` (atau modul kernel minimal) dengan API yang jelas (`mount`, `umount`, `open`, `read`, `write`, `delete`, `rename`, `fsync`).
2. `block_api.h` — definisi `read_blocks` / `write_blocks` / `fsync_device`.
3. Test suite: `tests/` (script untuk membuat disk img, mount, run tests).
4. `docs/IMPLEMENTATION.md` — detail algoritma, invariants, crash-recovery notes, and example flows.
5. `tools/fsck_fat32` — simple recovery tool that finds orphaned clusters and suggests fixes.

---

## Performance, limitations & constraints

* FAT32 has 4GB file size limit — jika agen ingin support >4GB, sarankan exFAT/ext4 sebagai alternatif.
* FAT32 is non-journaling — explain tradeoffs and optional simple transactional layer.
* Discuss fragmentation and propose simple defragmentation tool.

---

## Hints / References (agen bebas cari & verifikasi)

* Microsoft FAT32 specification (BPB/FAT layout) — gunakan sebagai ground truth
* OSDev FAT32 and Wiki guides for implementation detail
* Linux kernel block device and driver docs for DMA/IO semantics
* QEMU virtio-block for testing on VMs

---

## Acceptance criteria (how we accept the work)

* A disk image created by the agent can be mounted read/write on a modern Linux host and Windows host and roundtrip tests pass.
* The driver exposes `fsync` that guarantees data visible after return on a real block device (or simulated virtio with forced flush).
* Tests for crash-sim produce a documented recovery path and `fsck` can fix common inconsistencies.

---

## Extra credit (if completed)

* Implement a minimal LRU block cache with write-back and crash-consistent flush order.
* Provide a FUSE frontend that uses the implemented on-disk routines (allow user-space mounting for rapid iteration).
* Support exFAT (for >4GB files) as an additional module.

---

### Tone for the agent

Bekerjalah seperti engineer systems berpengalaman: berikan desain modular, dokumentasi yang jelas, dan test-first approach. Jika menemui ambiguities pada spec, tunjukkan pilihan desain dan trade-offs serta referensi yang mendukung pilihan tersebut.

---

*End of prompt.*
