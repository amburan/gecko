[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  xmax = 12
  ymax = 3
  zmax = 3
[]

[Variables]
  [./temp]
  [../]
[]

[Kernels]
  [./hc]
    type = HeatConduction
    variable = temp
  [../]
  [./td]
    type = TimeDerivative
    variable = temp
  [../]
[]

[BCs]
  active = 'left_ms right_ms'
  [./left]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 400
  [../]
  [./right]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 300
  [../]
  [./left_ms]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = left
    lammps_userobject = lammps_uo
  [../]
  [./right_ms]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = right
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  [./hcm]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 10
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    lammpsInput = /home/sthomas/lammps/lammps-ro/examples/COUPLE/simple/in.bar1d_flux
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 9
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  type = Exodus
  [./out_ms]
    type = Exodus
  [../]
[]

