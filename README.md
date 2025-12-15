# FinalProject-Struktur-data-dan-Analisa-Algoritma
Final Project mata kuliah Struktur Data dan Analisa Algoritma dengan dosen pengampu Pak Arta Kusuma Hernanda,

dibuat oleh Andre Adelfi Stevena

NRP 5024241089

simulasi ini menggunakan 2 algoritma untuk mengecek collision yaitu:

Brute Force dan Quadtree
Simulasi menampilkan partikel bergerak bebas yang saling bertabrakan

# Fitur
1.Pergerakan Partikel: Setiap partikel bergerak dengan kecepatan random yang berbeda-beda
2.Wall Collision: Partikel memantul ketika menabrak dinding dengan pembalikan arah velocity
3.Particle Collision: Deteksi dan resolusi tabrakan antar partikel menggunakan elastic collision physics
4.Boundary Correction: Posisi partikel otomatis disesuaikan agar tidak menembus dinding

# Kontrol Pada Program
| Tombol | Fungsi | Keterangan |
|----------|----------|----------|
| SPACE  | Add 5 Particles   | Menambahkan 5 partikel baru dengan posisi, ukuran, warna, dan kecepatan random   |
|Q  | Toggle Algorithm   | Switch antara algoritma Brute Force dan Quadtree   |
|V  | Toggle Grid   | Menampilkan/menyembunyikan grid pembagian Quadtree   |
|R | Reset   | Menghapus semua partikel dari layar   |

# Algoritma Yang Digunakan
1. Brute Force = Memeriksa setiap pasangan partikel yang ada untuk mendeteksi kemungkinan tabrakan.
2. Quadtree = Struktur data spatial partitioning yang membagi ruang 2D menjadi region-region yang lebih kecil secara hierarchical. Setiap node dapat dibagi menjadi 4 child nodes (northwest, northeast, southwest, southeast).

# Kesimpulan
Dari 2 algoritma yang digunakan dapat disimpulkan bahwa algoritma brute force lebih cocok untuk digunakan jika partikel lebih sedikit, karena kesederhanaannya. Sedangkan  algoritma quadtree cocok untuk digunakan pada simulasi yang partikelnya terdapat banyak karena lebih efisien dan lebih cepat dari algoritma brute force.
