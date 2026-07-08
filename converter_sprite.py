"""
Conversor de PNG para array C em formato RGB565.
Uso: python converter_sprite.py <imagem.png> <nome_do_array> <largura_destino> <altura_destino>

Gera um arquivo .h com o array de pixels pronto para usar no jogo da DE1-SoC.
"""
import sys
from PIL import Image

def rgb888_to_rgb565(r, g, b):
    """Converte um pixel RGB de 8 bits por canal para RGB565 (16 bits)."""
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def converter(caminho_img, nome_array, larg_dest, alt_dest, saida):
    img = Image.open(caminho_img).convert("RGB")
    img = img.resize((larg_dest, alt_dest), Image.LANCZOS)
    
    pixels = list(img.getdata())
    total = larg_dest * alt_dest
    
    with open(saida, "w") as f:
        f.write(f"#ifndef {nome_array.upper()}_H\n")
        f.write(f"#define {nome_array.upper()}_H\n\n")
        f.write(f"#define {nome_array.upper()}_W {larg_dest}\n")
        f.write(f"#define {nome_array.upper()}_H {alt_dest}\n\n")
        f.write(f"/* Gerado automaticamente a partir de: {caminho_img} */\n")
        f.write(f"static const unsigned short {nome_array}[{total}] = {{\n")
        
        for i, (r, g, b) in enumerate(pixels):
            valor = rgb888_to_rgb565(r, g, b)
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{valor:04X}")
            if i < total - 1:
                f.write(",")
            if i % 16 == 15 or i == total - 1:
                f.write("\n")
            else:
                f.write(" ")
        
        f.write("};\n\n")
        f.write(f"#endif\n")
    
    print(f"Convertido com sucesso! {total} pixels salvos em '{saida}'")
    print(f"Dimensoes: {larg_dest} x {alt_dest}")
    print(f"Tamanho do array: {total * 2} bytes ({total * 2 / 1024:.1f} KB)")

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Uso: python converter_sprite.py <imagem.png> <nome_array> <largura> <altura>")
        sys.exit(1)
    
    caminho = sys.argv[1]
    nome = sys.argv[2]
    largura = int(sys.argv[3])
    altura = int(sys.argv[4])
    arquivo_saida = sys.argv[5] if len(sys.argv) > 5 else f"{nome}.h"
    
    converter(caminho, nome, largura, altura, arquivo_saida)
