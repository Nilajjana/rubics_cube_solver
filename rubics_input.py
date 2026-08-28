import cv2
import numpy as np
from collections import Counter
from itertools import product
import sys
import os


# ============================================================
# FACE ORDER USED BY THIS PROGRAM
# ============================================================

USER_FACE_ORDER = "UFDRLB"

# Kociemba face order
KOCIEMBA_FACE_ORDER = "URFDLB"


# ============================================================
# KOCIEMBA CUBIE DEFINITIONS
#
# Corners:
#   0 URF
#   1 UFL
#   2 ULB
#   3 UBR
#   4 DFR
#   5 DLF
#   6 DBL
#   7 DRB
#
# Edges:
#   0 UR
#   1 UF
#   2 UL
#   3 UB
#   4 DR
#   5 DF
#   6 DL
#   7 DB
#   8 FR
#   9 FL
#   10 BL
#   11 BR
# ============================================================

U, R, F, D, L, B = range(6)

CORNER_FACELET = [
    [8,  9, 20],   # URF = U9 R1 F3
    [6, 18, 38],   # UFL = U7 F1 L3
    [0, 36, 47],   # ULB = U1 L1 B3
    [2, 45, 11],   # UBR = U3 B1 R3

    [29, 26, 15],  # DFR = D3 F9 R7
    [27, 44, 24],  # DLF = D1 L9 F7
    [33, 53, 42],  # DBL = D7 B9 L7
    [35, 17, 51],  # DRB = D9 R9 B7
]

EDGE_FACELET = [
    [5, 10],       # UR = U6 R2
    [7, 19],       # UF = U8 F2
    [3, 37],       # UL = U4 L2
    [1, 46],       # UB = U2 B2

    [32, 16],      # DR = D6 R8
    [28, 25],      # DF = D2 F8
    [30, 43],      # DL = D4 L8
    [34, 52],      # DB = D8 B8

    [23, 12],      # FR = F6 R4
    [21, 41],      # FL = F4 L6
    [50, 39],      # BL = B6 L4
    [48, 14],      # BR = B4 R6
]

CORNER_COLOR = [
    [U, R, F],     # URF
    [U, F, L],     # UFL
    [U, L, B],     # ULB
    [U, B, R],     # UBR

    [D, F, R],     # DFR
    [D, L, F],     # DLF
    [D, B, L],     # DBL
    [D, R, B],     # DRB
]

EDGE_COLOR = [
    [U, R],        # UR
    [U, F],        # UF
    [U, L],        # UL
    [U, B],        # UB

    [D, R],        # DR
    [D, F],        # DF
    [D, L],        # DL
    [D, B],        # DB

    [F, R],        # FR
    [F, L],        # FL
    [B, L],        # BL
    [B, R],        # BR
]


# ============================================================
# UTILITY: CONVERT UFDRLB -> URFDLB
# ============================================================

def ufdr_lb_to_kociemba(s):
    """
    Input:
        UFDRLB

    Output:
        URFDLB
    """

    if len(s) != 54:
        raise ValueError("Facelet string must contain exactly 54 characters.")

    faces = {
        "U": s[0:9],
        "F": s[9:18],
        "D": s[18:27],
        "R": s[27:36],
        "L": s[36:45],
        "B": s[45:54],
    }

    return (
        faces["U"] +
        faces["R"] +
        faces["F"] +
        faces["D"] +
        faces["L"] +
        faces["B"]
    )


# ============================================================
# PERMUTATION PARITY
# ============================================================

def permutation_parity(p):
    inversions = 0

    for i in range(len(p)):
        for j in range(i + 1, len(p)):
            if p[i] > p[j]:
                inversions += 1

    return inversions % 2


# ============================================================
# CONVERT FACELETS -> CP, CO, EP, EO
# ============================================================

def facelets_to_cubie(facelets):
    """
    facelets must be in Kociemba URFDLB order.

    Returns:
        cp, co, ep, eo
    """

    if len(facelets) != 54:
        raise ValueError("Need exactly 54 facelets.")

    # Convert letters to numerical face IDs
    face_id = {
        "U": U,
        "R": R,
        "F": F,
        "D": D,
        "L": L,
        "B": B
    }

    try:
        f = [face_id[x] for x in facelets]
    except KeyError as e:
        raise ValueError(
            f"Invalid facelet character: {e.args[0]}"
        )

    # --------------------------------------------------------
    # CORNERS
    # --------------------------------------------------------

    cp = []
    co = []

    for i in range(8):

        ori = 0

        # Find where U/D sticker is
        while (
            ori < 3
            and f[CORNER_FACELET[i][ori]] not in (U, D)
        ):
            ori += 1

        if ori == 3:
            raise ValueError(
                f"Corner {i} has no U/D sticker."
            )

        col1 = f[
            CORNER_FACELET[i][(ori + 1) % 3]
        ]

        col2 = f[
            CORNER_FACELET[i][(ori + 2) % 3]
        ]

        found = None

        for j in range(8):

            if (
                CORNER_COLOR[j][1] == col1
                and
                CORNER_COLOR[j][2] == col2
            ):
                found = j
                break

        if found is None:
            raise ValueError(
                f"Invalid corner colors at position {i}."
            )

        cp.append(found)
        co.append(ori)

    # --------------------------------------------------------
    # EDGES
    # --------------------------------------------------------

    ep = []
    eo = []

    for i in range(12):

        # Kociemba edge orientation
        if f[EDGE_FACELET[i][0]] in (U, D):
            ori = 0
        else:
            ori = 1

        col1 = f[
            EDGE_FACELET[i][ori % 2]
        ]

        col2 = f[
            EDGE_FACELET[i][(ori + 1) % 2]
        ]

        found = None

        for j in range(12):

            if (
                EDGE_COLOR[j][0] == col1
                and
                EDGE_COLOR[j][1] == col2
            ):
                found = j
                break

        if found is None:
            raise ValueError(
                f"Invalid edge colors at position {i}."
            )

        ep.append(found)
        eo.append(ori)

    return cp, co, ep, eo


# ============================================================
# VALIDATE KOCIEMBA STATE
# ============================================================

def validate_kociemba(facelets):
    """
    facelets must be Kociemba URFDLB.
    """

    if len(facelets) != 54:
        return False, "Length is not 54."

    counts = Counter(facelets)

    for c in KOCIEMBA_FACE_ORDER:

        if counts[c] != 9:
            return (
                False,
                f"Face {c} occurs {counts[c]} times; "
                f"must occur exactly 9 times."
            )

    try:
        cp, co, ep, eo = facelets_to_cubie(facelets)

    except ValueError as e:
        return False, str(e)

    # --------------------------------------------------------
    # Check duplicate/missing pieces
    # --------------------------------------------------------

    if sorted(cp) != list(range(8)):
        return False, f"Invalid CP permutation: {cp}"

    if sorted(ep) != list(range(12)):
        return False, f"Invalid EP permutation: {ep}"

    # --------------------------------------------------------
    # Corner orientation
    # --------------------------------------------------------

    corner_orientation_sum = sum(co)

    if corner_orientation_sum % 3 != 0:
        return (
            False,
            "Invalid corner orientation: "
            f"sum(co) = {corner_orientation_sum}"
        )

    # --------------------------------------------------------
    # Edge orientation
    # --------------------------------------------------------

    edge_orientation_sum = sum(eo)

    if edge_orientation_sum % 2 != 0:
        return (
            False,
            "Invalid edge orientation: "
            f"sum(eo) = {edge_orientation_sum}"
        )

    # --------------------------------------------------------
    # Permutation parity
    # --------------------------------------------------------

    cp_parity = permutation_parity(cp)
    ep_parity = permutation_parity(ep)

    if cp_parity != ep_parity:
        return (
            False,
            "Permutation parity mismatch: "
            f"CP parity={cp_parity}, "
            f"EP parity={ep_parity}"
        )

    return True, {
        "cp": cp,
        "co": co,
        "ep": ep,
        "eo": eo
    }


# ============================================================
# IMAGE PROCESSING
# ============================================================

def detect_stickers(image):
    """
    Detect the 54 square stickers from the black borders.

    Returns:
        list of:
            (x, y, w, h, center_x, center_y)
    """

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Black borders of the stickers
    mask = cv2.inRange(gray, 0, 80)

    contours, _ = cv2.findContours(
        mask,
        cv2.RETR_LIST,
        cv2.CHAIN_APPROX_SIMPLE
    )

    stickers = []

    for contour in contours:

        x, y, w, h = cv2.boundingRect(contour)

        # Expected size for each square
        if not (40 <= w <= 80 and 40 <= h <= 80):
            continue

        # Reject badly shaped objects
        if abs(w - h) > 5:
            continue

        area = cv2.contourArea(contour)

        if area < 2000:
            continue

        cx = x + w / 2
        cy = y + h / 2

        stickers.append(
            (x, y, w, h, cx, cy)
        )

    if len(stickers) != 54:
        raise ValueError(
            f"Expected 54 stickers, detected {len(stickers)}."
        )

    return stickers


# ============================================================
# GROUP THE 54 STICKERS INTO SIX 3x3 FACES
# ============================================================

def group_faces(stickers):
    """
    Expected cube-net:

              U
          L F R B
              D
    """

    # --------------------------------------------------------
    # Cluster stickers into the 9 horizontal rows
    # --------------------------------------------------------

    def cluster(values, tolerance=15):
        groups = []

        for v in sorted(values):

            if not groups:
                groups.append([v])

            elif abs(v - np.mean(groups[-1])) <= tolerance:
                groups[-1].append(v)

            else:
                groups.append([v])

        return [np.mean(g) for g in groups]

    y_centers = [s[5] for s in stickers]

    y_rows = cluster(y_centers)

    if len(y_rows) != 9:
        raise ValueError(
            f"Expected 9 sticker rows, found {len(y_rows)}."
        )

    # --------------------------------------------------------
    # Put every sticker into its horizontal row
    # --------------------------------------------------------

    rows = [[] for _ in range(9)]

    for sticker in stickers:

        row_index = min(
            range(9),
            key=lambda i:
                abs(sticker[5] - y_rows[i])
        )

        rows[row_index].append(sticker)

    # Sort each row from left to right
    for row in rows:
        row.sort(key=lambda s: s[4])

    # --------------------------------------------------------
    # Check cube-net structure
    #
    # Rows 0-2 : U
    # Rows 3-5 : L F R B
    # Rows 6-8 : D
    # --------------------------------------------------------

    for i in range(3):
        if len(rows[i]) != 3:
            raise ValueError(
                f"Top row {i} has {len(rows[i])} stickers."
            )

    for i in range(3, 6):
        if len(rows[i]) != 12:
            raise ValueError(
                f"Middle row {i} has {len(rows[i])} stickers."
            )

    for i in range(6, 9):
        if len(rows[i]) != 3:
            raise ValueError(
                f"Bottom row {i} has {len(rows[i])} stickers."
            )

    # --------------------------------------------------------
    # U face
    # --------------------------------------------------------

    U_face = (
        rows[0] +
        rows[1] +
        rows[2]
    )

    # --------------------------------------------------------
    # D face
    # --------------------------------------------------------

    D_face = (
        rows[6] +
        rows[7] +
        rows[8]
    )

    # --------------------------------------------------------
    # Middle row:
    #
    # L L L | F F F | R R R | B B B
    # --------------------------------------------------------

    L_face = []
    F_face = []
    R_face = []
    B_face = []

    for row in rows[3:6]:

        L_face.extend(row[0:3])
        F_face.extend(row[3:6])
        R_face.extend(row[6:9])
        B_face.extend(row[9:12])

    # --------------------------------------------------------
    # Return faces
    # --------------------------------------------------------

    result = {
        "U": U_face,
        "F": F_face,
        "D": D_face,
        "R": R_face,
        "L": L_face,
        "B": B_face,
    }

    # --------------------------------------------------------
    # Final sanity check
    # --------------------------------------------------------

    all_stickers = []

    for face in "UFDRLB":

        if len(result[face]) != 9:
            raise ValueError(
                f"Face {face} has "
                f"{len(result[face])} stickers."
            )

        all_stickers.extend(result[face])

    if len({id(s) for s in all_stickers}) != 54:
        raise ValueError(
            "Sticker grouping contains duplicate stickers."
        )

    return result

# ============================================================
# COLOR CLASSIFICATION
# ============================================================

def sample_sticker_color(image, sticker):
    """
    Take a small inner region rather than one pixel.
    """

    x, y, w, h, cx, cy = sticker

    # Avoid black border
    margin_x = int(w * 0.25)
    margin_y = int(h * 0.25)

    roi = image[
        y + margin_y:y + h - margin_y,
        x + margin_x:x + w - margin_x
    ]

    # Median is resistant to anti-aliasing
    median = np.median(
        roi.reshape(-1, 3),
        axis=0
    )

    return median


def classify_colors(image, faces):
    """
    The center of each face defines its face letter.

    Example:
        center of U face -> U
        center of F face -> F
        etc.

    Every other sticker is assigned to the closest
    center color.
    """

    center_colors = {}

    for face in USER_FACE_ORDER:

        center_sticker = faces[face][4]

        color = sample_sticker_color(
            image,
            center_sticker
        )

        center_colors[face] = color

    # Verify centers are sufficiently different
    face_names = list(USER_FACE_ORDER)

    for i in range(len(face_names)):
        for j in range(i + 1, len(face_names)):

            a = center_colors[face_names[i]]
            b = center_colors[face_names[j]]

            distance = np.linalg.norm(a - b)

            if distance < 30:
                raise ValueError(
                    "Two cube centers have almost the same color."
                )

    # Classify every sticker
    output = {}

    for face in USER_FACE_ORDER:

        chars = []

        for sticker in faces[face]:

            color = sample_sticker_color(
                image,
                sticker
            )

            best_face = min(
                USER_FACE_ORDER,
                key=lambda f:
                    np.linalg.norm(
                        color - center_colors[f]
                    )
            )

            chars.append(best_face)

        output[face] = ''.join(chars)

    return output, center_colors


# ============================================================
# IMAGE -> UFDRLB STRING
# ============================================================

def image_to_facelet_string(image_path):

    if not os.path.exists(image_path):
        raise FileNotFoundError(image_path)

    image = cv2.imread(image_path)

    if image is None:
        raise ValueError(
            "OpenCV could not read the image."
        )

    stickers = detect_stickers(image)

    faces = group_faces(stickers)

    face_strings, center_colors = classify_colors(
        image,
        faces
    )

    result = ''.join(
        face_strings[f]
        for f in USER_FACE_ORDER
    )

    return result, face_strings, center_colors


# ============================================================
# PRINT CUBIE STATE
# ============================================================

def print_cubie_state(cp, co, ep, eo):

    corner_names = [
        "URF", "UFL", "ULB", "UBR",
        "DFR", "DLF", "DBL", "DRB"
    ]

    edge_names = [
        "UR", "UF", "UL", "UB",
        "DR", "DF", "DL", "DB",
        "FR", "FL", "BL", "BR"
    ]

    print()
    print("CP:")
    print(cp)

    print("CO:")
    print(co)

    print("EP:")
    print(ep)

    print("EO:")
    print(eo)

    print()
    print("Corner pieces:")

    for i, p in enumerate(cp):
        print(
            f"  {corner_names[i]} -> "
            f"{corner_names[p]}   orientation={co[i]}"
        )

    print()
    print("Edge pieces:")

    for i, p in enumerate(ep):
        print(
            f"  {edge_names[i]} -> "
            f"{edge_names[p]}   orientation={eo[i]}"
        )


# ============================================================
# VALIDATE A USER-PROVIDED UFDRLB STRING
# ============================================================

def validate_user_string(s):

    s = s.strip().upper().replace(" ", "")

    print("Input:")
    print(s)
    print()

    if len(s) != 54:
        print(
            f"INVALID: length = {len(s)}, expected 54."
        )
        return

    bad = set(s) - set(USER_FACE_ORDER)

    if bad:
        print(
            f"INVALID: illegal characters: {bad}"
        )
        return

    # Convert to Kociemba's URFDLB
    kociemba_string = ufdr_lb_to_kociemba(s)

    print("Kociemba URFDLB representation:")
    print(kociemba_string)
    print()

    valid, result = validate_kociemba(
        kociemba_string
    )

    if not valid:

        print("❌ INVALID KOCIEMBA CUBE")
        print("Reason:", result)
        return

    print("✅ VALID KOCIEMBA CUBE")

    cp = result["cp"]
    co = result["co"]
    ep = result["ep"]
    eo = result["eo"]

    print_cubie_state(cp, co, ep, eo)


# ============================================================
# MAIN
# ============================================================

def main():

    if len(sys.argv) == 2:

        argument = sys.argv[1]

        # If argument looks like a 54-character facelet string
        cleaned = argument.strip().upper().replace(" ", "")

        if len(cleaned) == 54:
            validate_user_string(cleaned)
            return

        # Otherwise treat as image
        image_path = argument

        print("Reading image:", image_path)

        try:

            facelet_string, face_strings, centers = (
                image_to_facelet_string(image_path)
            )

        except Exception as e:

            print()
            print("❌ ERROR while reading image:")
            print(e)
            return

        print()
        print("Detected faces:")

        for face in USER_FACE_ORDER:
            print(
                f"{face}: {face_strings[face]}"
            )

        print()
        print("UFDRLB facelet string:")
        print(facelet_string)

        print()
        print("Kociemba URFDLB string:")

        kociemba_string = (
            ufdr_lb_to_kociemba(
                facelet_string
            )
        )

        print(kociemba_string)

        print()
        print("Validating...")

        valid, result = validate_kociemba(
            kociemba_string
        )

        if not valid:

            print()
            print("❌ INVALID KOCIEMBA CUBE")
            print("Reason:", result)

        else:

            print()
            print("✅ VALID KOCIEMBA CUBE")

            print_cubie_state(
                result["cp"],
                result["co"],
                result["ep"],
                result["eo"]
            )

        return

    # --------------------------------------------------------
    # Interactive mode
    # --------------------------------------------------------

    print("Rubik's Cube Image / Kociemba Validator")
    print()
    print("1. Enter image path")
    print("2. Enter UFDRLB facelet string")
    print()

    choice = input("Choice: ").strip()

    if choice == "1":

        path = input("Enter image path: ").strip()

        main_image = path

        try:

            facelet_string, face_strings, centers = (
                image_to_facelet_string(main_image)
            )

            print()
            print("Detected faces:")

            for face in USER_FACE_ORDER:
                print(
                    f"{face}: {face_strings[face]}"
                )

            print()
            print("UFDRLB:")
            print(facelet_string)

            kociemba_string = (
                ufdr_lb_to_kociemba(
                    facelet_string
                )
            )

            print()
            print("Kociemba URFDLB:")
            print(kociemba_string)

            valid, result = validate_kociemba(
                kociemba_string
            )

            if valid:

                print()
                print("✅ VALID KOCIEMBA CUBE")

                print_cubie_state(
                    result["cp"],
                    result["co"],
                    result["ep"],
                    result["eo"]
                )

            else:

                print()
                print("❌ INVALID KOCIEMBA CUBE")
                print("Reason:", result)

        except Exception as e:

            print()
            print("❌ ERROR:")
            print(e)

    elif choice == "2":

        s = input(
            "Enter UFDRLB 54-character string: "
        )

        validate_user_string(s)

    else:

        print("Invalid choice.")


if __name__ == "__main__":
    main()